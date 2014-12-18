#include "PixelPusher.h"
#include <algorithm>

PixelPusher::PixelPusher(DeviceHeader* header) {
  mArtnetUniverse = 0;
  mArtnetChannel = 0;
  mPort = 9798;
  mStripsAttached = 0;
  mPixelsPerStrip = 0;
  mExtraDelayMsec = 0;
  mMulticast = false;
  mMulticastPrimary = false;
  mAutothrottle = false;
  mSegments = 0;
  mPowerDomain = 0;
  mLastPingAt = ofGetElapsedTimef();
  mResetSentAt = ofGetElapsedTimef();
  mSendReset = false;

  mDeviceHeader = header;
  mPacket = header->getPacketRemainder();
  int packetLength = header->getPacketRemainderLength();

  if(packetLength < 28) {
    ofLog(OF_LOG_ERROR, "Packet size is too small! PixelPusher can't be created.");
  }

  memcpy(&mStripsAttached, &mPacket.get()[0], 1);
  memcpy(&mMaxStripsPerPacket, &mPacket.get()[1], 1);
  memcpy(&mPixelsPerStrip, &mPacket.get()[2], 2);
  memcpy(&mUpdatePeriod, &mPacket.get()[4], 4);
  memcpy(&mPowerTotal, &mPacket.get()[8], 4);
  memcpy(&mDeltaSequence, &mPacket.get()[12], 4);
  memcpy(&mControllerId, &mPacket.get()[16], 4);
  memcpy(&mGroupId, &mPacket.get()[20], 4);
  memcpy(&mArtnetUniverse, &mPacket.get()[24], 2);
  memcpy(&mArtnetChannel, &mPacket.get()[26], 2);

  if(packetLength < 28 && header->getSoftwareRevision() > 100) {
    memcpy(&mPort, &mPacket.get()[28], 2);
  }
  else {
    mPort = 9798; 
  }
  short defaultNumberOfStrips = 8;
  short stripFlagSize = std::max(mStripsAttached, defaultNumberOfStrips);
  mStripFlags.resize(stripFlagSize);
  
  if (packetLength > 30 && header->getSoftwareRevision() > 108) {
    memcpy(&mStripFlags[0], &mPacket.get()[30], stripFlagSize);
  }
  else {
    for (int i = 0; i < stripFlagSize; i++) {
      mStripFlags[i] = 0;
    }
  }

  if (packetLength > 30 + stripFlagSize && header->getSoftwareRevision() > 108 ) {
    // set Pusher flags
    long pusherFlags;
    memcpy(&pusherFlags, &mPacket.get()[32+stripFlagSize], 4);
    setPusherFlags(pusherFlags);
    memcpy(&mSegments, &mPacket.get()[36+stripFlagSize], 4);
    memcpy(&mPowerDomain, &mPacket.get()[40+stripFlagSize], 4);
  }
}

PixelPusher::~PixelPusher() {
  
}

int PixelPusher::getNumberOfStrips() {
  return mStrips.size();
}

std::deque<shared_ptr<Strip> > PixelPusher::getStrips() {
  return mStrips;
}

void PixelPusher::addStrip(shared_ptr<Strip> strip) {
  mStrips.push_back(strip);
}

shared_ptr<Strip> PixelPusher::getStrip(int stripNumber) {
  return mStrips.at(stripNumber);
}

int PixelPusher::getMaxStripsPerPacket() {
  return mMaxStripsPerPacket;
}

int PixelPusher::getPixelsPerStrip(int stripNumber) {
  return mStrips.at(stripNumber)->getLength();
}

void PixelPusher::setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue) {
  mStrips.at(stripNumber)->setPixels(red, green, blue);
}

void PixelPusher::setStripValues(int stripNumber, std::vector<shared_ptr<Pixel> > pixels) {
  mStrips.at(stripNumber)->setPixels(pixels);
}

std::string PixelPusher::getMacAddress() {
  return mDeviceHeader->getMacAddressString();
}

std::string PixelPusher::getIpAddress() {
  return mDeviceHeader->getIpAddressString();
}

/*
void PixelPusher::sendPacket() {
  long packetLength = 0;
  bool payload = false;
  std::deque<shared_ptr<Strip> > remainingStrips = getStrips();
  int stripsPerPacket = getMaxStripsPerPacket();

  while(!remainingStrips.empty()) {
    payload = false;
    memcpy(&mPacket[0], &mPacketNumber, 4);
    packetLength += 4;

    for(int i = 0; i < stripsPerPacket; i++) {
      if(remainingStrips.empty()) {
	break;
      }

      shared_ptr<Strip> strip = remainingStrips.front();
      if(strip->isTouched() ) {
        strip->serialize();
	shared_ptr<unsigned char> stripData = strip->serialize();
	int stripLength = strip->getLength();
	short stripNumber = strip->getStripNumber();
	memcpy(&mPacket[packetLength], stripNumber, 2);
	packetLength += 2;
	memcpy(&mPacket[packetLength], stripData, stripLength);
	payload = true;
      }
      remainingStrips.pop_front();
    }
    if(payload) {
      mPacketNumber++;
      payload = false;
      //      string packetMsg(packet.begin(), packet.end()); turn packetData, packetLength into a c_string
      //      udp.Send(packetMsg.c_str(), packetMsg.length());
    }
  }
}
*/

void PixelPusher::setPusherFlags(long pusherFlags) {
  mPusherFlags = pusherFlags; 
}

long PixelPusher::getGroupId() {
  return mGroupId;
}

long PixelPusher::getDeltaSequence() {
  return mDeltaSequence;
}

void PixelPusher::increaseExtraDelay(long delay) {
  mExtraDelayMsec += delay;
}

void PixelPusher::decreaseExtraDelay(long delay) {
  if (mExtraDelayMsec >= delay) {
    mExtraDelayMsec -= delay;
  }
  else {
    mExtraDelayMsec = 0;
  }
}

long PixelPusher::getExtraDelay() {
  return mExtraDelayMsec;
}

long PixelPusher::getUpdatePeriod() {
  return mUpdatePeriod;
}

short PixelPusher::getArtnetChannel() {
  return mArtnetChannel;
}

short PixelPusher::getArtnetUniverse() {
  return mArtnetUniverse;
}

short PixelPusher::getPort() {
  return mPort;
}

long PixelPusher::getPowerTotal() {
  return mPowerTotal;
}

long PixelPusher::getPowerDomain() {
  return mPowerDomain;
}

long PixelPusher::getSegments() {
  return mSegments;
}

long PixelPusher::getPusherFlags() {
  return mPusherFlags;
}

void PixelPusher::copyHeader(shared_ptr<PixelPusher> pusher) {
  mControllerId = pusher->mControllerId;
  mDeltaSequence = pusher->mDeltaSequence;
  mGroupId = pusher->mGroupId;
  mMaxStripsPerPacket = pusher->mMaxStripsPerPacket;
  mPowerTotal = pusher->mPowerTotal;
  mUpdatePeriod = pusher->mUpdatePeriod;
  mArtnetChannel = pusher->mArtnetChannel;
  mArtnetUniverse = pusher->mArtnetUniverse;
  mPort = pusher->mPort;
  setPusherFlags(pusher->getPusherFlags());
  mPowerDomain = pusher->mPowerDomain;
}

void PixelPusher::updateVariables(shared_ptr<PixelPusher> pusher) {
  mDeltaSequence = pusher->mDeltaSequence;
  mMaxStripsPerPacket = pusher->mMaxStripsPerPacket;
  mPowerTotal = pusher->mPowerTotal;
  mUpdatePeriod = pusher->mUpdatePeriod;
}

bool PixelPusher::isEqual(shared_ptr<PixelPusher> pusher) {
  long updatePeriodDifference = getUpdatePeriod() - pusher->getUpdatePeriod();
  if(abs(updatePeriodDifference) > 500) {
    return false;
  }

  //include check for color of strips
  
  if(getNumberOfStrips() != pusher->getNumberOfStrips()) {
    return false;
  }

  if(mArtnetChannel != pusher->getArtnetChannel() || mArtnetUniverse != pusher->getArtnetUniverse()) {
    return false;
  }

  if(mPort != pusher->getPort()) {
    return false;
  }

  long powerTotalDifference = mPowerTotal - pusher->getPowerTotal();
  if(abs(powerTotalDifference) > 10000) {
    return false;
  }

  if(getPowerDomain() != pusher->getPowerDomain()) {
    return false;
  }

  if(getSegments() != pusher->getSegments()) {
    return false;
  }

  if(getPusherFlags() != pusher->getPusherFlags()) {
    return false;
  }
}
