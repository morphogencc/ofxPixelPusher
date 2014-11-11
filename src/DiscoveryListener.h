/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 * 
 * A class that listens for the Universal Disocvery Protocol and reports the results.
 */

#pragma once
#include <iostream>
#include "ofMain.h"
#include "ofxNetwork.h"

class DiscoveryListener {
 public:
  DiscoveryListener();
  ~DiscoveryListener();
  void update();
 private:
  ofxUDPManager udpConnection;
  int newMessage;
  static const int incomingPacketSize = 76;
  static const int portNo = 7331;
};
