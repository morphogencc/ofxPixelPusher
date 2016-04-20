/*
 * DiscoveryService
 * Aug 2014
 * nathan lachenmyer
 */

#include <memory>
#include "DiscoveryService.h"
#include "DeviceHeader.h"

using namespace ofxPixelPusher;

std::shared_ptr<DiscoveryService> DiscoveryService::mDiscoveryService = nullptr;

std::shared_ptr<DiscoveryService> DiscoveryService::getInstance() {
	if (mDiscoveryService == nullptr) {
		mDiscoveryService = std::shared_ptr<DiscoveryService>(new DiscoveryService());
	}
	return mDiscoveryService;
}

int DiscoveryService::getFrameLimit() {
	return mFrameLimit;
}

std::shared_ptr<PixelPusher> DiscoveryService::getPusher(int controllerId) {
	for (auto pusherItem : mPusherMap) {
		if (pusherItem.second->getControllerId() == controllerId) {
			return pusherItem.second;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<PixelPusher> > DiscoveryService::getPushers() {
	mUpdateMutex.lock();
	std::vector<std::shared_ptr<PixelPusher> > pusherVector;
	for (std::map<std::string, std::shared_ptr<PixelPusher> >::iterator it = mPusherMap.begin();
	it != mPusherMap.end();
		++it) {
		pusherVector.push_back(it->second);
	}
	mUpdateMutex.unlock();
	return pusherVector;
}

std::vector<std::shared_ptr<PixelPusher> > DiscoveryService::getGroup(long groupId) {
	mUpdateMutex.lock();
	std::vector<std::shared_ptr<PixelPusher> > pusherVector;
	for (std::map<long, std::shared_ptr<PixelPusher> >::iterator it = mGroupMap.lower_bound(groupId);
	it != mGroupMap.upper_bound(groupId);
		++it) {
		pusherVector.push_back(it->second);
	}
	mUpdateMutex.unlock();
	return pusherVector;
}

std::shared_ptr<PixelPusher> DiscoveryService::getController(long groupId, long controllerId) {
	//only returns the FIRST pixelpusher with a particular group / controller number combination!
	mUpdateMutex.lock();
	for (std::map<long, std::shared_ptr<PixelPusher> >::iterator it = mGroupMap.lower_bound(groupId);
	it != mGroupMap.upper_bound(groupId);
		++it) {
		if (it->second->getControllerId() == controllerId) {
			mUpdateMutex.unlock();
			return it->second;
		}
	}
	//if no matching PixelPusher is found, return a nullPtr
	std::shared_ptr<PixelPusher> nullPtr;
	mUpdateMutex.unlock();
	return nullPtr;
}

void DiscoveryService::setLogLevel(LogLevel log_level) {
	mLogLevel = log_level;
	addRegistrationCallback([=](std::shared_ptr<PixelPusher> pusher) {
		pusher->setLogLevel(mLogLevel);
	});
}

LogLevel DiscoveryService::getLogLevel() {
	return mLogLevel;
}

void DiscoveryService::setPowerScale(double power_scale) {
	mPowerScale = power_scale;
	addRegistrationCallback([=](std::shared_ptr<PixelPusher> pusher) {
		pusher->setPowerScale(mPowerScale);
	});
}

double DiscoveryService::getPowerScale() {
	return mPowerScale;
}


DiscoveryService::DiscoveryService() {
	mDiscoveryServiceSocket = std::make_shared<ofxAsio::UdpReceiver>("0.0.0.0", 7331);
	mDiscoveryServiceSocket->addOnReceiveFn([=](std::shared_ptr<ofxAsio::Datagram> datagram) {
		DiscoveryService::getInstance()->update(datagram->getDataAsString());
	});
	mDiscoveryServiceSocket->start();
	std::printf("DiscoveryService::DiscoveryService -- Starting PixelPusher Discovery Service...\n");

	mAutoThrottle = true;
	mFrameLimit = 60;
	mLogLevel = PRODUCTION;
	mUpdateMapThread = std::thread(&DiscoveryService::updatePusherMap, this);
}

DiscoveryService::~DiscoveryService() {
	if (mUpdateMapThread.joinable()) {
		mUpdateMapThread.join();
	}
}

void DiscoveryService::update(std::string udpMessage) {
	if (mLogLevel == DEBUG) {
		std::printf("DiscoveryService::update -- Updating registry...\n");
	}
	mUpdateMutex.lock();
	DeviceHeader* header;

	header = new DeviceHeader(reinterpret_cast<const unsigned char*> (udpMessage.c_str()), udpMessage.length());
	if (header->getDeviceType() != PIXELPUSHER) {
		//if the device type isn't PixelPusher, end processing it right here.
		return;
	}

	std::shared_ptr<PixelPusher> incomingDevice(new PixelPusher(header));
	std::string macAddress = incomingDevice->getMacAddress();
	std::string ipAddress = incomingDevice->getIpAddress();
	mLastSeenMap[macAddress] = std::clock() / CLOCKS_PER_SEC;

	if (mPusherMap.count(macAddress) == 0) {
		//does not already exist in the map
		addNewPusher(macAddress, incomingDevice);
		std::printf("DiscoveryService::update -- Adding new PixelPusher %s at address %s\n", macAddress.c_str(), ipAddress.c_str());
	}
	else {
		//already exists in the map
		if (!mPusherMap[macAddress]->isEqual(incomingDevice)) {
			//if the pushers are not equal, replace it with this one
			updatePusher(macAddress, incomingDevice);
			if (mLogLevel == DEBUG) {
				std::printf("DiscoveryService::update -- Updating PixelPusher %s at address %s\n", macAddress.c_str(), ipAddress.c_str());
			}
		}
		else {
			//if they're the same, then just update it
			mPusherMap[macAddress]->updateVariables(incomingDevice);
			if (mLogLevel == DEBUG) {
				std::printf("DiscoveryService::update -- Updating PixelPusher %s at address %s\n", macAddress.c_str(), ipAddress.c_str());
			}
			if (incomingDevice->getDeltaSequence() > 3) {
				mPusherMap[macAddress]->increaseExtraDelay(5);
			}
			if (incomingDevice->getDeltaSequence() < 1) {
				mPusherMap[macAddress]->decreaseExtraDelay(1);
			}
		}
	}

	mUpdateMutex.unlock();
}

void DiscoveryService::addNewPusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher) {
	mPusherMap.insert(std::make_pair(macAddress, pusher));
	mGroupMap.insert(std::make_pair(pusher->getGroupId(), pusher));
	pusher->createCardThread();
	for (auto callback : mRegistrationCallbacks) {
		callback(pusher);
	}
}

void DiscoveryService::updatePusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher) {
	mPusherMap[macAddress]->copyHeader(pusher);
}

void DiscoveryService::updatePusherMap() {
	mRunUpdateMapThread = true;
	while (mRunUpdateMapThread) {
		mUpdateMutex.lock();
		for (std::map<std::string, std::shared_ptr<PixelPusher> >::iterator pusher = mPusherMap.begin(); pusher != mPusherMap.end();) {
			//pusher->first is Mac Address, pusher->second is the shared pointer to the PixelPusher
			if (!pusher->second->isAlive()) {
				for (auto callback : mRemovalCallbacks) {
					callback(pusher->second);
				}
				std::printf("DiscoveryService::updatePusherMap -- Removing PixelPusher %s from all maps.\n", pusher->first.c_str());
				pusher->second->destroyCardThread();
				//remove from multimap -- more complicated
				for (auto it = mGroupMap.lower_bound(pusher->second->getGroupId()); it != mGroupMap.end();) {
					if (it->second->isEqual(pusher->second)) {
						mGroupMap.erase(it++);
					}
					else {
						++it;
					}
				}
				//remove pusher from maps
				mLastSeenMap.erase(pusher->first);
				mPusherMap.erase(pusher++);
			}
			else {
				++pusher;
			}
		}
		mUpdateMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void DiscoveryService::addRegistrationCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function) {
  mRegistrationCallbacks.push_back(callback_function);
}

void DiscoveryService::addRemovalCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function) {
  mRemovalCallbacks.push_back(callback_function);
}
