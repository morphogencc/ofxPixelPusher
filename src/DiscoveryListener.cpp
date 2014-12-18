/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 */

#include <tr1/memory>
#include "DiscoveryListener.h"
#include "DeviceHeader.h"
#include "PixelPusher.h"

using namespace std::tr1;

DiscoveryListener::DiscoveryListener() {
  udpConnection.Create();
  udpConnection.SetEnableBroadcast(true);
  //  udpConnection.BindMcast("0.0.0.0",portNo);
  udpConnection.Bind(portNo);
  udpConnection.SetNonBlocking(true);

  newMessage = 0;
  incomingUdpMessage.resize(incomingPacketSize + 1);
  std::fill(incomingUdpMessage.begin(), incomingUdpMessage.end(), 0);
  udpMessage.resize(incomingPacketSize + 1);
  std::fill(udpMessage.begin(), udpMessage.end(), 0);
  
  mAutoThrottle = true;
}

void DiscoveryListener::update() {
  //this should be its own thread
  newMessage = udpConnection.Receive(&incomingUdpMessage[0],incomingPacketSize); //returns number of bytes received
  std::copy(incomingUdpMessage.begin(), incomingUdpMessage.end(), udpMessage.begin());

  if(newMessage != -1) {
    DeviceHeader* header;
    header = new DeviceHeader(&udpMessage[0], incomingPacketSize);
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
	ofLog(OF_LOG_NOTICE, "updating PixelPusher  " + macAddress + " " + ipAddress);
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
