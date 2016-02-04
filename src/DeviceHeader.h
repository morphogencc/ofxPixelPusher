#pragma once

#include <memory>
#include <string>
#include <stdio.h>
#include <iostream>

enum DeviceType {
  ETHERDREAM, 
  LUMIABRIDGE, 
  PIXELPUSHER
};

class DeviceHeader {
 public:
  DeviceHeader(const unsigned char* packet, int packetLength);
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
  std::shared_ptr<unsigned char> getPacketRemainder();
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
  std::shared_ptr<unsigned char> mPacketRemainder;
  int mPacketRemainderLength;
};
