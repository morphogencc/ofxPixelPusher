#include "DeviceHeader.h"
#include "ofMain.h"

using namespace ofxPixelPusher;

DeviceHeader::DeviceHeader(const unsigned char* packet, int packetLength) {
  if(packetLength < sHeaderLength) {
    ofLogError() << "DeviceHeader::DeviceHeader ERROR -- Incorrect package length in ofxPixelPusher::DeviceHeader constructor!";
  }

  memcpy(&mMacAddress[0], &packet[0], 6);
  memcpy(&mIpAddress[0], &packet[6], 4);
  mDeviceType = static_cast<DeviceType>(packet[10]);
  mProtocolVersion = packet[11];
  memcpy(&mVendorId, &packet[12], 2);
  memcpy(&mProductId, &packet[14], 2);
  memcpy(&mHardwareRevision, &packet[16], 2);
  memcpy(&mSoftwareRevision, &packet[18], 2);
  memcpy(&mLinkSpeed, &packet[20], 4);

  if(mSoftwareRevision < mOldestAcceptableSoftwareRevision) {
    ofLogError("ofxPixelPusher") << "This PixelPusher Library requires firmware revision " << mOldestAcceptableSoftwareRevision / 100.0;
    ofLogError("ofxPixelPusher") << "This PixelPusher is using " << mSoftwareRevision / 100.0;
    ofLogError("ofxPixelPusher") << "This is not expected to work.  Please update your PixelPusher.";
  }
        
  mPacketRemainderLength = packetLength - sHeaderLength;
  mPacketRemainder = std::shared_ptr<unsigned char>(new unsigned char[mPacketRemainderLength]);
  memcpy(&mPacketRemainder.get()[0], &packet[sHeaderLength], mPacketRemainderLength);
}

DeviceHeader::~DeviceHeader() {
  ofLogNotice() << "DeviceHeader deconstructed";
}

std::string DeviceHeader::getMacAddressString() {
  char strMacAddress[24];
  sprintf(strMacAddress, "%02X:%02X:%02X:%02X:%02X:%02X", mMacAddress[0], mMacAddress[1], mMacAddress[2], mMacAddress[3], mMacAddress[4], mMacAddress[5]);
  return strMacAddress;
}

std::string DeviceHeader::getIpAddressString() {
  char strIpAddress[24];
  sprintf(strIpAddress, "%u.%u.%u.%u", mIpAddress[0], mIpAddress[1], mIpAddress[2], mIpAddress[3]);
  return strIpAddress;
}

DeviceType DeviceHeader::getDeviceType() {
  return mDeviceType;
}

unsigned char DeviceHeader::getProtocolVersion() {
  return mProtocolVersion;
}

short DeviceHeader::getVendorId() {
  return mVendorId;
}

short DeviceHeader::getProductId() {
  return mProductId;
}

short DeviceHeader::getHardwareRevision() {
  return mHardwareRevision;
}

short DeviceHeader::getSoftwareRevision() {
  return mSoftwareRevision;
}

long DeviceHeader::getLinkSpeed() {
  return mLinkSpeed;
}

std::shared_ptr<unsigned char> DeviceHeader::getPacketRemainder() {
  return mPacketRemainder;
}

int DeviceHeader::getPacketRemainderLength() {
  return mPacketRemainderLength;
}

bool DeviceHeader::isMulticast() {
  if(mIpAddress[0] >= 224 && mIpAddress[0] <= 239) {
    return true;
  }
  else {
    return false;
  }
}
