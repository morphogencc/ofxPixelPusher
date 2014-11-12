#pragma once

#include <string>
#include <tr1/memory>
#include <stdio.h>
#include "ByteUtils.h"

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
  char mMacAddress[6];
  char mIpAddress[4];
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
