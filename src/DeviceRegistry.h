/*
 * DeviceRegistry
 * Aug 2014
 * nathan lachenmyer
 *
 * registry of PP devices
 */

#pragma once
#include <map>
#include "Observable.h"

class DeviceRegistry : public Observable {
 public:
  DeviceRegistry();
  ~DeviceRegistry();
  //void expireDevice(String macAddr);
  //void setStripValues(String macAddr, int stripNumber, Pixel[] pixels);
  //void startPushing();
  //void stopPushing();
  void receive(byte* data);
  //vector<Strip> getStrips();
  //vector<Strip> getStrips(int groupNumber);
 private:
  //void updatePusher(String macAddr, PixelPusher device);
  //void addNewPusher(String macAddr, PixelPusher pusher);
  
  //map<String, PixelPusher*> pusherMap;
  //map<String, DateTime> pusherLastSeenMap;
  static int DISCOVERY_PORT;
  static int MAX_DISCONNECT_SECONDS;
  static long EXPIRY_TIMER_MSEC;
  static long totalPower;
  static long totalPowerLimit;
  static double powerScale;
  static boolean autoThrottle;
  
}
