/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 * 
 * A class that listens for the Universal Disocvery Protocol and reports the results.
 *
 */

#pragma once

#ifdef _WIN32
#include <memory>
#elif __APPLE__
#include <tr1/memory>
#endif

#include <iostream>
#include <map>
#include <utility>
#include "ofMain.h"
#include "ofxNetwork.h"
#include "PixelPusher.h"

using namespace std::tr1;

class DiscoveryListener : public ofThread {
 public:
  static DiscoveryListener* getInstance();
  void freeInstance();
  int getFrameLimit();
  std::vector<shared_ptr<PixelPusher> > getPushers();
  std::vector<shared_ptr<PixelPusher> > getGroup(long groupId);
  shared_ptr<PixelPusher> getController(long groupId, long controllerId);
 private:
  DiscoveryListener();
  ~DiscoveryListener();
  void threadedFunction();
  void update();
  void addNewPusher(std::string macAddress, shared_ptr<PixelPusher> pusher);
  void updatePusher(std::string macAddress, shared_ptr<PixelPusher> pusher);
  void updatePusherMap();
  static DiscoveryListener* mDiscoveryThread;
  ofxUDPManager mUdpConnection;
  int mMessageFlag;
  std::vector<char> mIncomingUdpMessage;
  std::vector<unsigned char> mUdpMessage;
  static const int mIncomingPacketSize = 76;
  static const int mPort = 7331;
  bool mAutoThrottle;
  int mFrameLimit;
  std::map<std::string, shared_ptr<PixelPusher> > mPusherMap;
  std::map<std::string, long> mLastSeenMap;
  std::multimap<long, shared_ptr<PixelPusher> > mGroupMap;
};
