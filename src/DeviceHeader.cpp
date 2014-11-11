#include "DeviceHeader.h"

DeviceHeader::DeviceHeader(unsigned char* packet[], int packetLength) {
  if(packetLength < sHeaderLength) {
    std::cout << "Incorrect package length in DeviceHeader constructor!";
  }

  memcpy(&mMacAddress[0], &packet[0], 6);
  memcpy(&mIpAddress[0], &packet[6], 4);
  mDeviceType = static_cast<DeviceType>(*packet[10]);
  mProtocolVersion = *packet[11];
  memcpy(&mVendorId, &packet[12], 2);
  memcpy(&mProductId, &packet[14], 2);
  memcpy(&mHardwareRevision, &packet[16], 2);
  memcpy(&mSoftwareRevision, &packet[18], 2);
  memcpy(&mLinkSpeed, &packet[20], 4);
        
  mPacketRemainderLength = packetLength - sHeaderLength;
  mPacketRemainder = shared_ptr<uint8_t>( new uint8_t[mPacketRemainderLength]);
  memcpy(&mPacketRemainder.get()[0], &packet[sHeaderLength], mPacketRemainderLength);

  /*
    strncpy(this->macAddress, (char*)packet, 6);
    intf("Mac Addres: %s", this->macAddress);
    strncpy(this->ipAddress, (char*)packet+6, 4);
    printf("IP Addres: %s", this->ipAddress);
    this->deviceType = (DeviceType)packet[10];
    this->protocolVersion = (int)packet[11];
    this->vendorId = ByteUtils::unsignedCharToShort(packet,12,13);
    this->productId = ByteUtils::unsignedCharToShort(packet,14,15);
    this->hardwareRevision = ByteUtils::unsignedCharToShort(packet,16,17);
    this->softwareRevision = ByteUtils::unsignedCharToShort(packet,18,19);
    this->linkSpeed = ByteUtils::unsignedCharToLong(packet,20,23);
    strncpy(this->packetRemainder, (char*)packet + 24, (76 - 24));
  */
}

DeviceHeader::~DeviceHeader() {
  
}

std::string DeviceHeader::getMacAddressString() {
  char strMacAddress[24];
  sprintf(strMacAddress, "0x%02X:0x%02X:0x%02X:0x%02X:0x%02X:0x%02X\n", mMacAddress[0], mMacAddress[1], mMacAddress[2], mMacAddress[3], mMacAddress[4], mMacAddress[5]);
  return strMacAddress;
}

std::string DeviceHeader::getIpAddressString() {
  char strIpAddress[24];
  sprintf(strIpAddress, "%d.%d.%d.%d\n", mIpAddress[0], mIpAddress[1], mIpAddress[2], mIpAddress[3]);
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

shared_ptr<unsigned char> DeviceHeader::getPacketRemainder() {
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
