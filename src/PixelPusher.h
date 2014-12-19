/*
 * PixelPusher
 * Dec 2014
 * nathan lachenmyer
 * http://morphogen.cc/
 */

#pragma once

#include <vector>
#include <deque>
#include <list>
#include <string>
#include <tr1/memory>
#include "Strip.h"
#include "ofMain.h"
#include "ofxNetwork.h"
#include "DeviceHeader.h"

using namespace std::tr1;

class PixelPusher : public ofThread {
 public:
  PixelPusher(DeviceHeader* header);
  ~PixelPusher();
  int getNumberOfStrips();
  std::deque<shared_ptr<Strip> > getStrips();
  void addStrip(shared_ptr<Strip> strip);
  shared_ptr<Strip> getStrip(int stripNumber);
  int getMaxStripsPerPacket();
  int getPixelsPerStrip(int stripNumber);
  void setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue);
  void setStripValues(int stripNumber, std::vector<shared_ptr<Pixel> > pixels);
  std::string getMacAddress();
  std::string getIpAddress();
  //void sendPacket();
  long getGroupId();
  long getDeltaSequence();
  void increaseExtraDelay(long delay);
  void decreaseExtraDelay(long delay);
  long getExtraDelay();
  long getUpdatePeriod();
  short getArtnetChannel();
  short getArtnetUniverse();
  short getPort();
  long getPowerTotal();
  long getPowerDomain();
  long getSegments();
  void setPusherFlags(long pusherFlags);
  long getPusherFlags();
  void copyHeader(shared_ptr<PixelPusher> pusher);
  void updateVariables(shared_ptr<PixelPusher> pusher);
  bool isEqual(shared_ptr<PixelPusher> pusher);
  bool isAlive();
 private:
  void configureNetwork();
  void threadedFunction();
  static const int mTimeoutTime = 5;
  ofxUdpConnection mUdpConnection;
  long mPusherFlags;
  DeviceHeader* mDeviceHeader;
  long mPacketNumber;
  shared_ptr<unsigned char> mPacket;
  short mPort;
  short mStripsAttached;
  short mMaxStripsPerPacket;
  short mPixelsPerStrip;
  long mUpdatePeriod;
  long mPowerTotal;
  long mDeltaSequence;
  long mControllerId;
  long mGroupId;
  short mArtnetUniverse;
  short mArtnetChannel;
  long mExtraDelayMsec;
  bool mMulticast;
  bool mMulticastPrimary;
  bool mAutothrottle;
  long mSegments;
  long mPowerDomain;
  double mLastPingAt;
  double mResetSentAt;
  bool mSendReset;
  std::vector<unsigned char> mStripFlags;
  std::deque<shared_ptr<Strip> > mStrips;
};
