/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 */


#include <memory>
#include "DiscoveryListener.h"
#include "DeviceHeader.h"

DiscoveryListener* DiscoveryListener::mDiscoveryThread = NULL;

DiscoveryListener* DiscoveryListener::getInstance() {
  if(mDiscoveryService == NULL) {
    mDiscoveryService = new DiscoveryListener();
  }
  return mDiscoveryService;
}

void DiscoveryListener::freeInstance() {
  delete mDiscoveryService;
  mDiscoveryService = NULL;
}

int DiscoveryListener::getFrameLimit() {
  return mFrameLimit;
}

std::vector<shared_ptr<PixelPusher> > DiscoveryListener::getPushers() {
  this->lock();
  std::vector<shared_ptr<PixelPusher> > pusherVector;
  for(std::map<std::string, shared_ptr<PixelPusher> >::iterator it = mPusherMap.begin();
	it != mPusherMap.end();
      ++it) {
    pusherVector.push_back(it->second);
  }
  this->unlock();
  return pusherVector;
}

std::vector<shared_ptr<PixelPusher> > DiscoveryListener::getGroup(long groupId) {
  this->lock();
  std::vector<shared_ptr<PixelPusher> > pusherVector;
  for(std::map<long, shared_ptr<PixelPusher> >::iterator it = mGroupMap.lower_bound(groupId);
	it != mGroupMap.upper_bound(groupId);
      ++it) {
    pusherVector.push_back(it->second);
  }
  this->unlock();
  return pusherVector;
}

shared_ptr<PixelPusher> DiscoveryListener::getController(long groupId, long controllerId) {
  this->lock();
  for(std::map<long, shared_ptr<PixelPusher> >::iterator it = mGroupMap.lower_bound(groupId);
	it != mGroupMap.upper_bound(groupId);
      ++it) {
    if(it->second->getControllerId() == controllerId) {
      this->unlock();
      return it->second;
    }
  }
  //if no matching PixelPusher is found, return a nullPtr
  shared_ptr<PixelPusher> nullPtr;
  this->unlock();
  return nullPtr;
}

DiscoveryListener::DiscoveryListener() {
  mUdpConnection.Create();
  mUdpConnection.SetEnableBroadcast(true);
  mUdpConnection.Bind(mPort);
  mUdpConnection.SetNonBlocking(true);
  ofLog(OF_LOG_NOTICE, "Listening for UDP messages on port %d.", mPort);
  mMessageFlag = 0;
  mIncomingUdpMessage.resize(mIncomingPacketSize + 1);
  std::fill(mIncomingUdpMessage.begin(), mIncomingUdpMessage.end(), 0);
  mUdpMessage.resize(mIncomingPacketSize + 1);
  std::fill(mUdpMessage.begin(), mUdpMessage.end(), 0);
  
  mAutoThrottle = true;
  mFrameLimit = 60;

  mUpdateMapThread = std::thread(&DiscoveryListener::updatePusherMap, this);
}

DiscoveryListener::~DiscoveryListener() {
  if(mUpdateMapThread.joinable()) {
    mUpdateMapThread.join();
  }
}

void DiscoveryListener::update() {
  ofLog(OF_LOG_NOTICE, "Updating registry...");
  mMessageFlag = mUdpConnection.Receive(&mIncomingUdpMessage[0],mIncomingPacketSize);

  if(mMessageFlag != -1) {
    mUpdateMutex.lock();
    mUdpMessage.clear();
    ofLog(OF_LOG_NOTICE, "Received packet, processing...");
    std::copy(mIncomingUdpMessage.begin(), mIncomingUdpMessage.end(), mUdpMessage.begin());
    DeviceHeader* header;
    header = new DeviceHeader(&mUdpMessage[0], mIncomingPacketSize);
    if(header->getDeviceType() != PIXELPUSHER) {
      //if the device type isn't PixelPusher, end processing it right here.
      return;
    }
    
    shared_ptr<PixelPusher> incomingDevice(new PixelPusher(header));
    std::string macAddress = incomingDevice->getMacAddress();
    std::string ipAddress = incomingDevice->getIpAddress();
    mLastSeenMap[macAddress] = ofGetElapsedTimef();

    if(mPusherMap.count(macAddress) == 0) {
      //does not already exist in the map
      addNewPusher(macAddress, incomingDevice);
      ofLog(OF_LOG_NOTICE, "Adding new PixelPusher " + macAddress + " " + ipAddress);
    }
    else {
      //already exists in the map
      if(!mPusherMap[macAddress]->isEqual(incomingDevice)) {
	//if the pushers are not equal, replace it with this one
	updatePusher(macAddress, incomingDevice);
	ofLog(OF_LOG_NOTICE, "Updating PixelPusher  " + macAddress + " " + ipAddress);
      }
      else {
	//if they're the same, then just update it
	mPusherMap[macAddress]->updateVariables(incomingDevice);
	ofLog(OF_LOG_NOTICE, "Updating PixelPusher  " + macAddress + " " + ipAddress);
	if(incomingDevice->getDeltaSequence() > 3) {
	  mPusherMap[macAddress]->increaseExtraDelay(5);
	}
	if(incomingDevice->getDeltaSequence() < 1) {
	  mPusherMap[macAddress]->decreaseExtraDelay(1);
	}
      }
    }
    
    mUpdateMutex.unlock();
  }
}

void DiscoveryListener::addNewPusher(std::string macAddress, shared_ptr<PixelPusher> pusher) {
  mPusherMap.insert(std::make_pair(macAddress, pusher));
  mGroupMap.insert(std::make_pair(pusher->getGroupId(), pusher));
  pusher->createCardThread();
}

void DiscoveryListener::updatePusher(std::string macAddress, shared_ptr<PixelPusher> pusher) {
  mPusherMap[macAddress]->copyHeader(pusher);
}

void DiscoveryListener::updatePusherMap() {
  mRunUpdateMapThread = true;
  while(mRunUpdateMapThread) {
    mUpdateMutex.lock();
    for(std::map<std::string, shared_ptr<PixelPusher> >::iterator pusher = mPusherMap.begin();
	pusher != mPusherMap.end();
	) {
      //pusher->first is Mac Address, pusher->second is the shared pointer to the PixelPusher
      if(!pusher->second->isAlive()) {
	ofLog(OF_LOG_NOTICE, "DiscoveryListener removing PixelPusher %s from all maps.", pusher->first.c_str());
	pusher->second->destroyCardThread();
	typedef std::multimap<long, shared_ptr<PixelPusher> >::iterator mapIterator;
	std::pair<mapIterator, mapIterator> iteratorPair = mGroupMap.equal_range(pusher->second->getGroupId());
      mapIterator it MyClass& MyClass::operator=(const MyClass &rhs) {
	// Check for self-assignment!
	if (this == &rhs)
	  return *this;
	
	return *this;
      } iteratorPair.first;
      for(; it != iteratorPair.second; ++it) {
	if(it->second == pusher->second) {
	  mGroupMap.erase(it);
	  break;
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
    this_thread::sleep_for(std::chrono::milliseconds(1000));
  }  
}
