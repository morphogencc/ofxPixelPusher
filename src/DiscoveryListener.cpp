/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 */

#include <tr1/memory>
#include "DiscoveryListener.h"
#include "DeviceHeader.h"

DiscoveryListener* DiscoveryListener::mDiscoveryThread = NULL;

DiscoveryListener* DiscoveryListener::getInstance() {
  if(mDiscoveryThread == NULL) {
    mDiscoveryThread = new DiscoveryListener();
  }
  return mDiscoveryThread;
}

void DiscoveryListener::freeInstance() {
  delete mDiscoveryThread;
  mDiscoveryThread = NULL;
}

DiscoveryListener::DiscoveryListener() : ofThread() {
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
  this->startThread();
}

DiscoveryListener::~DiscoveryListener() {
  this->stopThread();
}

void DiscoveryListener::threadedFunction() {
  while(this->isThreadRunning()) {
    this->lock();
    update();
    this->unlock();
  }
}

void DiscoveryListener::update() {
  //this should be its own thread
  mMessageFlag = mUdpConnection.Receive(&mIncomingUdpMessage[0],mIncomingPacketSize); //returns number of bytes received
  std::copy(mIncomingUdpMessage.begin(), mIncomingUdpMessage.end(), mUdpMessage.begin());

  if(mMessageFlag != -1) {
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
    
    /*
    if ( TotalPowerLimit > 0 ) {
      TotalPower = 0;
      
      for(size_t k=0; k < mPushers.size(); k++) {
        TotalPower += mPushers[k]->getPowerTotal();
	}
      PowerScale = ( TotalPower > TotalPowerLimit ) ? ( TotalPowerLimit / TotalPower ) : 1.0;
    }
    */
  }
}

void DiscoveryListener::addNewPusher(std::string macAddress, shared_ptr<PixelPusher> pusher) {
  mPusherMap.insert(std::make_pair(macAddress, pusher));
  mGroupMap.insert(std::make_pair(pusher->getGroupId(), pusher));
  //pusher->createCardThread( mIoService );
}

void DiscoveryListener::updatePusher(std::string macAddress, shared_ptr<PixelPusher> pusher) {
  mPusherMap[macAddress]->copyHeader(pusher);
}

void DiscoveryListener::updatePusherMap() {
  //THREAD THIS FUNCTION
  for(std::map<std::string, shared_ptr<PixelPusher> >::iterator pusher = mPusherMap.begin();
      pusher != mPusherMap.end();
      ) {
    //pusher->first is Mac Address, pusher->second is the shared pointer to the PixelPusher
    if(!pusher->second->isAlive()) {
      ofLog(OF_LOG_NOTICE, "DiscoveryListener removing PixelPusher %s from all maps.", pusher->first.c_str());
      //remove pusher from group map
      typedef std::multimap<long, shared_ptr<PixelPusher> >::iterator mapIterator;
      std::pair<mapIterator, mapIterator> iteratorPair = mGroupMap.equal_range(pusher->second->getGroupId());
      mapIterator it = iteratorPair.first;
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
}
