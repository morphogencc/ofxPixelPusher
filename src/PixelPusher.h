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
#include "ofMain.h"
#include "ofxNetwork.h"
#include "Strip.h"
#include "DeviceHeader.h"

using namespace std::tr1;

class PixelPusher : public ofThread {
 public:
  PixelPusher(DeviceHeader* header);
  ~PixelPusher();
  int getNumberOfStrips();
  std::deque<shared_ptr<Strip> > getStrips();
  std::deque<shared_ptr<Strip> > getTouchedStrips();
  shared_ptr<Strip> getStrip(int stripNumber);
  void addStrip(shared_ptr<Strip> strip);
  int getMaxStripsPerPacket();
  int getPixelsPerStrip(int stripNumber);
  void setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue);
  void setStripValues(int stripNumber, std::vector<shared_ptr<Pixel> > pixels);
  std::string getMacAddress();
  std::string getIpAddress();
  long getGroupId();
  long getControllerId();
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
  void createCardThread();
  void destroyCardThread();
 private:
  void threadedFunction();
  void createStrips();
  void sendPacket();
  static const int mTimeoutTime = 5;
  static const int mFrameLimit = 60;
  ofxUDPManager mUdpConnection;
  long mPusherFlags;
  DeviceHeader* mDeviceHeader;
  long mPacketNumber;
  //unsigned char* mPacket;
  std::vector<unsigned char> mPacket;
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
  long mThreadDelay;
  long mThreadExtraDelay;
  long mTotalDelay;
  std::vector<unsigned char> mStripFlags;
  std::deque<shared_ptr<Strip> > mStrips;
};
