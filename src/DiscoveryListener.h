/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 * 
 * A class that listens for the Universal Disocvery Protocol and reports the results.
 *
 */

#pragma once

#include <memory>
#include <iostream>
#include <map>
#include <utility>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include "sdfWindows.hpp"
#include "sdfServerSocket.hpp"
#include "PixelPusher.h"

class DiscoveryListener {
 public:
  static DiscoveryListener* getInstance();
  void freeInstance();
  int getFrameLimit();
  std::vector<std::shared_ptr<PixelPusher> > getPushers();
  std::vector<std::shared_ptr<PixelPusher> > getGroup(long groupId);
  std::shared_ptr<PixelPusher> getController(long groupId, long controllerId);
 private:
  DiscoveryListener();
  ~DiscoveryListener();
  void update();
  void addNewPusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher);
  void updatePusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher);
  void updatePusherMap();
  static DiscoveryListener* mDiscoveryService;
	sdfServerSocket* mUdpConnection;
  int mMessageFlag;
  std::vector<char> mIncomingUdpMessage;
  std::vector<unsigned char> mUdpMessage;
  static const int mIncomingPacketSize = 76;
  static const int mPort = 7331;
  bool mAutoThrottle;
  bool mRunUpdateMapThread;
  int mFrameLimit;
  std::map<std::string, std::shared_ptr<PixelPusher> > mPusherMap;
  std::map<std::string, long> mLastSeenMap;
  std::multimap<long, std::shared_ptr<PixelPusher> > mGroupMap;
  std::thread mUpdateMapThread;
  std::mutex mUpdateMutex;
};
