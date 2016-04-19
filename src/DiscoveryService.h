/*
 * DiscoveryService
 * Aug 2014
 * nathan lachenmyer
 *
 * A class that listens for the Universal Disocvery Protocol and reports the results.
 *
 */

#pragma once

#include <memory>
#include <iostream>
#include <map>
#include <utility>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include "ofxAsio.h"
#include "PixelPusher.h"
#include "Utilities.h"

namespace ofxPixelPusher {

	class DiscoveryService {
	public:
		~DiscoveryService();
		static std::shared_ptr<DiscoveryService> getInstance();
		int getFrameLimit();
		std::shared_ptr<PixelPusher> getPusher(int controllerId);
		std::vector<std::shared_ptr<PixelPusher> > getPushers();
		std::vector<std::shared_ptr<PixelPusher> > getGroup(long groupId);
		std::shared_ptr<PixelPusher> getController(long groupId, long controllerId);
		void setLogLevel(LogLevel log_level);
		LogLevel getLogLevel();
		void setPowerScale(double power_scale);
		double getPowerScale();
		void addRegistrationCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function);
		void addRemovalCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function);
	private:
		DiscoveryService();
		void update(std::string udpMessage);
		void addNewPusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher);
		void updatePusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher);
		void updatePusherMap();
		static std::shared_ptr<DiscoveryService> mDiscoveryService;
		std::shared_ptr<ofxAsio::UdpReceiver> mDiscoveryServiceSocket;
		int mMessageFlag;
		static const int mIncomingPacketSize = 76;
		static const int mPort = 7331;
		bool mAutoThrottle;
		bool mRunUpdateMapThread;
		int mFrameLimit;
		double mPowerScale;
		LogLevel mLogLevel;
		std::map<std::string, std::shared_ptr<PixelPusher> > mPusherMap;
		std::map<std::string, long> mLastSeenMap;
		std::multimap<long, std::shared_ptr<PixelPusher> > mGroupMap;
		std::thread mUpdateMapThread;
		std::mutex mUpdateMutex;
		std::vector<std::function<void(std::shared_ptr<PixelPusher>)>> mRegistrationCallbacks;
		std::vector<std::function<void(std::shared_ptr<PixelPusher>)>> mRemovalCallbacks;
	};

}
