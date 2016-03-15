/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 */

#include <memory>
#include "DiscoveryListener.h"
#include "DeviceHeader.h"

using namespace ofxPixelPusher;

std::shared_ptr<DiscoveryListener> DiscoveryListener::mDiscoveryService = nullptr;

std::shared_ptr<DiscoveryListener> DiscoveryListener::getInstance() {
	if (mDiscoveryService == nullptr) {
		mDiscoveryService = std::shared_ptr<DiscoveryListener>(new DiscoveryListener());
	}
	return mDiscoveryService;
}

int DiscoveryListener::getFrameLimit() {
	return mFrameLimit;
}

std::vector<std::shared_ptr<PixelPusher> > DiscoveryListener::getPushers() {
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

std::vector<std::shared_ptr<PixelPusher> > DiscoveryListener::getGroup(long groupId) {
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

std::shared_ptr<PixelPusher> DiscoveryListener::getController(long groupId, long controllerId) {
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

DiscoveryListener::DiscoveryListener() {
	mDiscoveryServiceSocket = std::make_shared<ofxAsio::UdpReceiver>("0.0.0.0", 7331);
	mDiscoveryServiceSocket->addOnReceiveFn([=](std::shared_ptr<ofxAsio::Message> datagram) {
		DiscoveryListener::getInstance()->update(datagram->getMessage());
	});
	mDiscoveryServiceSocket->start();
	std::printf("Starting Discovery Listener Service...\n");

	mAutoThrottle = true;
	mFrameLimit = 60;

	mUpdateMapThread = std::thread(&DiscoveryListener::updatePusherMap, this);
}

DiscoveryListener::~DiscoveryListener() {
	if (mUpdateMapThread.joinable()) {
		mUpdateMapThread.join();
	}
}

void DiscoveryListener::update(std::string udpMessage) {
	std::printf("Updating registry...\n");
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
		std::printf("Adding new PixelPusher %s at address %s\n", macAddress.c_str(), ipAddress.c_str());
	}
	else {
		//already exists in the map
		if (!mPusherMap[macAddress]->isEqual(incomingDevice)) {
			//if the pushers are not equal, replace it with this one
			updatePusher(macAddress, incomingDevice);
			std::printf("Updating PixelPusher %s at address %s\n", macAddress.c_str(), ipAddress.c_str());
		}
		else {
			//if they're the same, then just update it
			mPusherMap[macAddress]->updateVariables(incomingDevice);
			std::printf("Updating PixelPusher %s at address %s\n", macAddress.c_str(), ipAddress.c_str());
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

void DiscoveryListener::addNewPusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher) {
	mPusherMap.insert(std::make_pair(macAddress, pusher));
	mGroupMap.insert(std::make_pair(pusher->getGroupId(), pusher));
	pusher->createCardThread();
}

void DiscoveryListener::updatePusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher) {
	mPusherMap[macAddress]->copyHeader(pusher);
}

void DiscoveryListener::updatePusherMap() {
	mRunUpdateMapThread = true;
	while (mRunUpdateMapThread) {
		mUpdateMutex.lock();
		for (std::map<std::string, std::shared_ptr<PixelPusher> >::iterator pusher = mPusherMap.begin(); pusher != mPusherMap.end();) {
			//pusher->first is Mac Address, pusher->second is the shared pointer to the PixelPusher
			if (!pusher->second->isAlive()) {
				std::printf("DiscoveryListener removing PixelPusher %s from all maps.\n", pusher->first.c_str());
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

void DiscoveryListener::addRegistrationCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function) {
  mRegistrationCallbacks.push_back(callback_function);
}

void DiscoveryListener::addRemovalCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function) {
  mRemovalCallbacks.push_back(callback_function);
}
