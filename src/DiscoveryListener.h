/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 * 
 * A class that listens for the Universal Disocvery Protocol and reports the results.
 */

#pragma once
#include <iostream>
#include <tr1/memory>
#include <map>
#include <utility>
#include "ofMain.h"
#include "ofxNetwork.h"
#include "PixelPusher.h"

using namespace std::tr1;

class DiscoveryListener {
 public:
  DiscoveryListener();
  ~DiscoveryListener();
  void update();
 private:
  void addNewPusher(std::string macAddress, shared_ptr<PixelPusher> pusher);
  void updatePusher(std::string macAddress, shared_ptr<PixelPusher> pusher);
  ofxUDPManager udpConnection;
  int newMessage;
  std::vector<char> incomingUdpMessage;
  std::vector<unsigned char> udpMessage;
  static const int incomingPacketSize = 76;
  static const int portNo = 7331;
  bool mAutoThrottle;
  std::map<std::string, shared_ptr<PixelPusher> > mPusherMap;
  std::map<std::string, long> mLastSeenMap;
  std::multimap<long, shared_ptr<PixelPusher> > mGroupMap;
  //DiscoveryThread discover;
  //SceneThread scene;
};
