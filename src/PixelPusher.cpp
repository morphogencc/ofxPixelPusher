#include "PixelPusher.h"
#include <algorithm>

PixelPusher::PixelPusher(DeviceHeader* header) {
  mArtnetUniverse = 0;
  mArtnetChannel = 0;
  mPort = 9897;
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
  shared_ptr<unsigned char> packetRemainder = header->getPacketRemainder();
  int packetLength = header->getPacketRemainderLength();

  if(packetLength < 28) {
    ofLog(OF_LOG_ERROR, "Packet size is too small! PixelPusher can't be created.");
  }

  memcpy(&mStripsAttached, &packetRemainder.get()[0], 1);
  memcpy(&mMaxStripsPerPacket, &packetRemainder.get()[1], 1);
  memcpy(&mPixelsPerStrip, &packetRemainder.get()[2], 2);
  memcpy(&mUpdatePeriod, &packetRemainder.get()[4], 4);
  memcpy(&mPowerTotal, &packetRemainder.get()[8], 4);
  memcpy(&mDeltaSequence, &packetRemainder.get()[12], 4);
  memcpy(&mControllerId, &packetRemainder.get()[16], 4);
  memcpy(&mGroupId, &packetRemainder.get()[20], 4);
  memcpy(&mArtnetUniverse, &packetRemainder.get()[24], 2);
  memcpy(&mArtnetChannel, &packetRemainder.get()[26], 2);

  if(packetLength < 28 && header->getSoftwareRevision() > 100) {
    memcpy(&mPort, &packetRemainder.get()[28], 2);
  }
  else {
    mPort = 9897; 
  }
  short defaultNumberOfStrips = 8;
  short stripFlagSize = std::max(mStripsAttached, defaultNumberOfStrips);
  mStripFlags.resize(stripFlagSize);
  
  if (packetLength > 30 && header->getSoftwareRevision() > 108) {
    memcpy(&mStripFlags[0], &packetRemainder.get()[30], stripFlagSize);
  }
  else {
    for (int i = 0; i < stripFlagSize; i++) {
      mStripFlags[i] = 0;
    }
  }

  if (packetLength > 30 + stripFlagSize && header->getSoftwareRevision() > 108 ) {
    // set Pusher flags
    long pusherFlags;
    memcpy(&pusherFlags, &packetRemainder.get()[32+stripFlagSize], 4);
    setPusherFlags(pusherFlags);
    memcpy(&mSegments, &packetRemainder.get()[36+stripFlagSize], 4);
    memcpy(&mPowerDomain, &packetRemainder.get()[40+stripFlagSize], 4);
  }

  mPacket.reserve(400*mMaxStripsPerPacket); //too high, can be reduced
}

PixelPusher::~PixelPusher() {
}

int PixelPusher::getNumberOfStrips() {
  return mStrips.size();
}

std::deque<shared_ptr<Strip> > PixelPusher::getStrips() {
  return mStrips;
}

std::deque<shared_ptr<Strip> > PixelPusher::getTouchedStrips() {
  std::deque<shared_ptr<Strip> > touchedStrips;
  for(auto strip : mStrips) {
    if(strip->isTouched()) {
      touchedStrips.push_back(strip);
    }
  }
  return touchedStrips;
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

void PixelPusher::sendPacket() {
  std::deque<shared_ptr<Strip> > remainingStrips = getTouchedStrips();
  bool payload = false;
  long packetLength = 0;
  mThreadDelay = 16.0;
  mPacket.clear();

  if(getUpdatePeriod() > 100000.0) {
    mThreadDelay = (16.0 / (mStripsAttached / mMaxStripsPerPacket));
  }
  else if(getUpdatePeriod() > 1000.0) {
    mThreadDelay = (getUpdatePeriod() / 1000.0) + 1;
  }
  else {
    mThreadDelay = ((1000.0 / mFrameLimit) / (mStripsAttached / mMaxStripsPerPacket));
  }
  
  mTotalDelay = mThreadDelay + mThreadExtraDelay + mExtraDelayMsec;
  
  ofLog(OF_LOG_NOTICE, "Total delay for PixelPusher %s is %ld", getMacAddress().c_str(), mTotalDelay);

  if(!mSendReset && remainingStrips.empty()) {
    this->sleep(mTotalDelay);
  }

  /*
    else if (mSendReset) {
    ofLog(OF_LOG_NOTICE, "Resetting PixelPusher %s at %s", getMacAddress().c_str(), getIpAddress().c_str());
    // update Packet number
    memcpy(&resetCmdData[0], &mPacketNumber, 4);
    
    // send packet
    mUdpConnection->Send(resetCmdBuffer);
    mPacketNumber++;
    
    mSendReset = false;
    return;
  }
  */

  while(!remainingStrips.empty()) {
    ofLog(OF_LOG_NOTICE, "Sending data to PixelPusher %s at %s:%d", getMacAddress().c_str(), getIpAddress().c_str(), mPort);
    payload = false;
    packetLength = 0;
    memcpy(&mPacket[0], &mPacketNumber, 4);
    packetLength += 4;
    
    for(int i = 0; i < mMaxStripsPerPacket; i++) {
      ofLog(OF_LOG_NOTICE, "Packing strip %d of %hd...", i, mMaxStripsPerPacket);

      if(remainingStrips.empty()) {
	break;
      }
      
      shared_ptr<Strip> strip = remainingStrips.front();
      strip->serialize();
      unsigned char* stripData = strip->getPixelData();
      int stripDataLength = strip->getPixelDataLength();
      short stripNumber = strip->getStripNumber();
      memcpy(&mPacket[packetLength], &stripNumber, 2);
      packetLength += 2;
      memcpy(&mPacket[packetLength], &stripData, stripDataLength);
      packetLength += stripDataLength;
      payload = true;
      remainingStrips.pop_front();
    }
    
    if(payload) {
      ofLog(OF_LOG_NOTICE, "Payload confirmed; sending packet of %d bytes", mPacket.size());
      for(int i = 0; i < packetLength; i++) {
	ofLog(OF_LOG_NOTICE, "%02X", mPacket[i]);
      }
      mPacketNumber++;
      mPacket.resize(packetLength);
      mUdpConnection.Send(reinterpret_cast<char *>(mPacket.data()), packetLength);
      payload = false;
    }
  }

  ofLog(OF_LOG_NOTICE, "Closing Card Thread for PixelPusher %s", getMacAddress().c_str());
}

void PixelPusher::setPusherFlags(long pusherFlags) {
  mPusherFlags = pusherFlags; 
}

long PixelPusher::getGroupId() {
  return mGroupId;
}

long PixelPusher::getControllerId() {
  return mControllerId;
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

bool PixelPusher::isAlive() {
  if((ofGetElapsedTimef() - mLastPingAt) < mTimeoutTime) {
    return true;
  }
  else {
    return false;
  }
}

void PixelPusher::createStrips() {
  for(int i = 0; i < mStripsAttached; i++) {
    shared_ptr<Strip> newStrip(new Strip(i, mPixelsPerStrip));
    mStrips.push_back(newStrip);
  }
}

void PixelPusher::createCardThread() {
  createStrips();
  mUdpConnection.Create();
  mUdpConnection.Connect(getIpAddress().c_str(), mPort);
  mUdpConnection.SetNonBlocking(true);
  ofLog(OF_LOG_NOTICE, "Connected to PixelPusher %s on port %d", getIpAddress().c_str(), mPort);
  mPacketNumber = 0;
  mThreadExtraDelay = 0;
  this->startThread();
}

void PixelPusher::destroyCardThread() {
  this->stopThread();
}

void PixelPusher::threadedFunction() {
  while(this->isThreadRunning()) {
    this->lock();
    sendPacket();
    this->unlock();
    this->sleep(mTotalDelay);
  }
}
