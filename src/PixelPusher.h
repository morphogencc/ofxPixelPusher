/*
* PixelPusher
* Dec 2014
* nathan lachenmyer
* http://morphogen.cc/
*/

#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <list>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include "Strip.h"
#include "DeviceHeader.h"
#include "Asio.h"

namespace ofxPixelPusher {

	enum LogLevel {
		PRODUCTION,
		DEBUG
	};

	class PixelPusher {
	public:
		PixelPusher(DeviceHeader* header);
		~PixelPusher();
		int getNumberOfStrips();
		std::deque<std::shared_ptr<Strip> > getStrips();
		std::deque<std::shared_ptr<Strip> > getTouchedStrips();
		std::shared_ptr<Strip> getStrip(int stripNumber);
		int getMaxStripsPerPacket();
		int getPixelsPerStrip(int stripNumber);
		void setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue);
		void setStripValues(int stripNumber, std::vector<std::shared_ptr<Pixel> > pixels);
		void setPowerScale(double powerscale);
		void setPowerScale(int stripNumber, double powerscale);
		void setColorTemperature(Pixel::ColorTemperature temperature);
		void setColorCorrection(Pixel::ColorCorrection correction);
		void setAntilog(bool antilog);
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
		void copyHeader(std::shared_ptr<PixelPusher> pusher);
		void updateVariables(std::shared_ptr<PixelPusher> pusher);
		bool isEqual(std::shared_ptr<PixelPusher> pusher);
		bool isAlive();
		void createCardThread();
		void destroyCardThread();
		void setLogLevel(LogLevel log_level);
		LogLevel getLogLevel();
	protected:
		void createStrips();
		void addStrip(std::shared_ptr<Strip> strip);
		void sendPacket();
		static const int mTimeoutTime = 5;
		static const int mFrameLimit = 60;
		long mPusherFlags;
		DeviceHeader* mDeviceHeader;
		long mPacketNumber;
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
		bool mRunCardThread;
		LogLevel mLogLevel;
		std::thread mCardThread;
		std::shared_ptr<ofxAsio::sockets::UdpSender> mCardThreadSender;
		std::vector<unsigned char> mStripFlags;
		std::deque<std::shared_ptr<Strip> > mStrips;
	};

}