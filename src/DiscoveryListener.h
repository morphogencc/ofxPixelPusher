/*
 * DiscoveryListener
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

	class DiscoveryListener {
	public:
		~DiscoveryListener();
		static std::shared_ptr<DiscoveryListener> getInstance();
		int getFrameLimit();
		std::vector<std::shared_ptr<PixelPusher> > getPushers();
		std::vector<std::shared_ptr<PixelPusher> > getGroup(long groupId);
		std::shared_ptr<PixelPusher> getController(long groupId, long controllerId);
		void addRegistrationCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function);
		void addRemovalCallback(std::function<void(std::shared_ptr<PixelPusher>)> callback_function);
	private:
		DiscoveryListener();
		void update(std::string udpMessage);
		void addNewPusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher);
		void updatePusher(std::string macAddress, std::shared_ptr<PixelPusher> pusher);
		void updatePusherMap();
		static std::shared_ptr<DiscoveryListener> mDiscoveryService;
		std::shared_ptr<ofxAsio::UdpReceiver> mDiscoveryServiceSocket;
		int mMessageFlag;
		static const int mIncomingPacketSize = 76;
		static const int mPort = 7331;
		bool mAutoThrottle;
		bool mRunUpdateMapThread;
		int mFrameLimit;
		std::map<std::string, std::shared_ptr<PixelPusher> > mPusherMap;
		std::map<std::string, long> mLastSeenMap;
		std::multimap<long, std::shared_ptr<PixelPusher> > mGroupMap;
		std::thread mUpdateMapThread;
		std::mutex mUpdateMutex;
		std::vector<std::function<void(std::shared_ptr<PixelPusher>)>> mRegistrationCallbacks;
		std::vector<std::function<void(std::shared_ptr<PixelPusher>)>> mRemovalCallbacks;
	};

}
