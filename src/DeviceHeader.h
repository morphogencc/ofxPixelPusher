#pragma once

#ifdef _WIN32
#include <memory>
#elif __APPLE__
#include <tr1/memory>
#endif

#include <string>
#include <stdio.h>
#include <iostream>
#include "ofMain.h"

using namespace std::tr1;

enum DeviceType {
  ETHERDREAM, 
  LUMIABRIDGE, 
  PIXELPUSHER
};

class DeviceHeader {
 public:
  DeviceHeader(unsigned char* packet, int packetLength);
  ~DeviceHeader();
  std::string getMacAddressString();
  std::string getIpAddressString();
  DeviceType getDeviceType();
  unsigned char getProtocolVersion();
  short getVendorId();
  short getProductId();
  short getHardwareRevision();
  short getSoftwareRevision();
  long getLinkSpeed();
  shared_ptr<unsigned char> getPacketRemainder();
  int getPacketRemainderLength();
  bool isMulticast();
 private:
  static const int sHeaderLength = 24;
  static const int mOldestAcceptableSoftwareRevision = 121;
  unsigned char mMacAddress[6];
  unsigned char mIpAddress[4];
  DeviceType mDeviceType;
  unsigned char mProtocolVersion;
  short mVendorId;
  short mProductId;
  short mHardwareRevision;
  short mSoftwareRevision;
  long mLinkSpeed;
  shared_ptr<unsigned char> mPacketRemainder;
  int mPacketRemainderLength;
};
