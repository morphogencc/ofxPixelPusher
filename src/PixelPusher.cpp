#include "PixelPusher.h"
#include <algorithm>
#include <ctime>
#include <iterator>

using namespace ofxPixelPusher;

PixelPusher::PixelPusher(DeviceHeader* header) {
	mArtnetUniverse = 0;
	mArtnetChannel = 0;
	mPort = 9897;
	mStripsAttached = 0;
	mPixelsPerStrip = 0;
	mExtraDelayMsec = 0;
	mMaxStripsPerPacket = 0;
	mUpdatePeriod = 0;
	mPowerTotal = 0;
	mDeltaSequence = 0;
	mControllerId = 0;
	mGroupId = 0;
	mSegments = 0;
	mPowerDomain = 0;
	mPacketNumber = 0;
	mThreadDelay = 0;
	mThreadExtraDelay = 0;
	mTotalDelay = 0;
	mRunCardThread = NULL;
	mLogLevel = PRODUCTION;

	mMulticast = false;
	mMulticastPrimary = false;
	mAutothrottle = false;
	mLastPingAt = std::clock() / CLOCKS_PER_SEC;
	mResetSentAt = std::clock() / CLOCKS_PER_SEC;
	mSendReset = false;

	mDeviceHeader = header;
	std::shared_ptr<unsigned char> packetRemainder = header->getPacketRemainder();
	int packetLength = header->getPacketRemainderLength();

	if (packetLength < 28) {
		std::printf("Packet size is too small! PixelPusher can't be created.\n");
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

	if (packetLength < 28 && header->getSoftwareRevision() > 100) {
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

	if (packetLength > 30 + stripFlagSize && header->getSoftwareRevision() > 108) {
		// set Pusher flags
		long pusherFlags;
		memcpy(&pusherFlags, &packetRemainder.get()[32 + stripFlagSize], 4);
		setPusherFlags(pusherFlags);
		memcpy(&mSegments, &packetRemainder.get()[36 + stripFlagSize], 4);
		memcpy(&mPowerDomain, &packetRemainder.get()[40 + stripFlagSize], 4);
	}

	mPacket.reserve(400 * mMaxStripsPerPacket); //too high, can be reduced
}

PixelPusher::~PixelPusher() {
}

int PixelPusher::getNumberOfStrips() {
	return mStrips.size();
}

std::deque<std::shared_ptr<Strip> > PixelPusher::getStrips() {
	return mStrips;
}

std::deque<std::shared_ptr<Strip> > PixelPusher::getTouchedStrips() {
	std::deque<std::shared_ptr<Strip> > touchedStrips;
	for (auto strip : mStrips) {
		if (strip->isTouched()) {
			touchedStrips.push_back(strip);
		}
	}
	return touchedStrips;
}

void PixelPusher::addStrip(std::shared_ptr<Strip> strip) {
	mStrips.push_back(strip);
}

std::shared_ptr<Strip> PixelPusher::getStrip(int stripNumber) {
	if (stripNumber < mStrips.size()) {
		return mStrips.at(stripNumber);
	}
	else {
		std::printf("PixelPusher::getStrip ERROR -- Invalid strip number %d.  Returning empty strip.\n", stripNumber);
		return std::shared_ptr<Strip>();
	}
}

int PixelPusher::getMaxStripsPerPacket() {
	return mMaxStripsPerPacket;
}

int PixelPusher::getPixelsPerStrip(int stripNumber) {
	if (stripNumber < mStrips.size()) {
		return mStrips.at(stripNumber)->getLength();
	}
	else {
		std::printf("PixelPusher::getPixelsPerStrip ERROR -- Invalid strip number %d.\n", stripNumber);
		return 0;
	}
}

void PixelPusher::setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue) {
	if (stripNumber < mStrips.size()) {
		mStrips.at(stripNumber)->setPixels(red, green, blue);
	}
	else {
		std::printf("PixelPusher::setStripValues ERROR -- Invalid strip number %d.\n", stripNumber);
	}
}

void PixelPusher::setStripValues(int stripNumber, std::vector<std::shared_ptr<Pixel> > pixels) {
	if (stripNumber < mStrips.size()) {
		mStrips.at(stripNumber)->setPixels(pixels);
	}
	else {
		std::printf("PixelPusher::setStripValues ERROR -- Invalid strip number %d.\n", stripNumber);
	}
}

void PixelPusher::setPowerScale(double powerScale) {
	for (auto strip : mStrips) {
		strip->setPowerScale(powerScale);
	}
}

void PixelPusher::setPowerScale(int stripNumber, double powerScale) {
	if (stripNumber < mStrips.size()) {
		mStrips.at(stripNumber)->setPowerScale(powerScale);
	}
	else {
		std::printf("PixelPusher::setPowerScale ERROR -- Invalid strip number %d.\n", stripNumber);
	}
}

void PixelPusher::setColorTemperature(Pixel::ColorTemperature temperature) {
	for (auto strip : mStrips) {
		strip->setColorTemperature(temperature);
	}
}

void PixelPusher::setColorCorrection(Pixel::ColorCorrection correction) {
	for (auto strip : mStrips) {
		strip->setColorCorrection(correction);
	}
}

void PixelPusher::setAntilog(bool antilog) {
	for (auto strip : mStrips) {
		for (auto pixel : strip->getPixels()) {
			pixel->setAntiLog(antilog);
		}
	}
}

std::string PixelPusher::getMacAddress() {
	return mDeviceHeader->getMacAddressString();
}

std::string PixelPusher::getIpAddress() {
	return mDeviceHeader->getIpAddressString();
}

void PixelPusher::sendPacket() {
	bool payload = false;
	mThreadDelay = 16.0;
	mPacket.clear();
	mRunCardThread = true;

	while (mRunCardThread) {
		std::deque<std::shared_ptr<Strip> > remainingStrips = getTouchedStrips();

		if (getUpdatePeriod() > 100000.0) {
			mThreadDelay = (16.0 / (mStripsAttached / mMaxStripsPerPacket));
		}
		else if (getUpdatePeriod() > 1000.0) {
			mThreadDelay = (getUpdatePeriod() / 1000.0) + 1;
		}
		else {
			mThreadDelay = ((1000.0 / mFrameLimit) / (mStripsAttached / mMaxStripsPerPacket));
		}

		mTotalDelay = mThreadDelay + mThreadExtraDelay + mExtraDelayMsec;

		if (mLogLevel == DEBUG) {
			std::printf("PixelPusher::sendPacket -- Updating total delay for PixelPusher %s to %ld", getMacAddress().c_str(), mTotalDelay);
		}

		if (!mSendReset && remainingStrips.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(mTotalDelay));
		}

		while (!remainingStrips.empty()) {
			if (mLogLevel == DEBUG) {
				std::printf("PixelPusher::sendPacket -- Sending data to PixelPusher %s at %s:%d", getMacAddress().c_str(), getIpAddress().c_str(), mPort);
			}
			payload = false;
			mPacket.clear();

			mPacket.push_back((mPacketNumber >> 24) & 0xFF);
			mPacket.push_back((mPacketNumber >> 16) & 0xFF);
			mPacket.push_back((mPacketNumber >> 8) & 0xFF);
			mPacket.push_back(mPacketNumber & 0xFF);

			for (int i = 0; i < mMaxStripsPerPacket; i++) {
				if (remainingStrips.empty()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(mTotalDelay));
					continue;
				}

				std::shared_ptr<Strip> strip = remainingStrips.front();
				strip->serialize();
				short stripNumber = strip->getStripNumber();
				//mPacket.push_back((stripNumber >> 8) & 0xFF);
				mPacket.push_back(stripNumber & 0xFF);

				std::copy(strip->begin(), strip->end(), std::back_inserter(mPacket));
				//copy doesn't seem to add stuff to mPacket...
				payload = true;
				remainingStrips.pop_front();
			}

			if (payload) {
				if (mLogLevel == DEBUG) {
					std::printf("PixelPusher::sendPacket -- Payload confirmed; sending packet of %lu bytes", mPacket.size());
				}
				mPacketNumber++;

				std::shared_ptr<ofxAsio::Datagram> packet = std::make_shared<ofxAsio::Datagram>(mPacket, getIpAddress(), getPort());
				mCardThreadSender->send(packet);
				payload = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(mTotalDelay));
			}
		}
	}

	if (mLogLevel == DEBUG) {
		std::printf("PixelPusher::sendPacket -- Closing Card Thread for PixelPusher %s\n", getMacAddress().c_str());
	}
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

void PixelPusher::copyHeader(std::shared_ptr<PixelPusher> pusher) {
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
	mLastPingAt = std::clock() / CLOCKS_PER_SEC;
}

void PixelPusher::updateVariables(std::shared_ptr<PixelPusher> pusher) {
	mDeltaSequence = pusher->mDeltaSequence;
	mMaxStripsPerPacket = pusher->mMaxStripsPerPacket;
	mPowerTotal = pusher->mPowerTotal;
	mUpdatePeriod = pusher->mUpdatePeriod;
	mLastPingAt = std::clock() / CLOCKS_PER_SEC;
}

bool PixelPusher::isEqual(std::shared_ptr<PixelPusher> pusher) {
	long updatePeriodDifference = getUpdatePeriod() - pusher->getUpdatePeriod();
	if (abs(updatePeriodDifference) > 500) {
		return false;
	}

	if (getNumberOfStrips() != pusher->getNumberOfStrips()) {
		return false;
	}

	if (mArtnetChannel != pusher->getArtnetChannel() || mArtnetUniverse != pusher->getArtnetUniverse()) {
		return false;
	}

	if (mPort != pusher->getPort()) {
		return false;
	}

	long powerTotalDifference = mPowerTotal - pusher->getPowerTotal();
	if (abs(powerTotalDifference) > 10000) {
		return false;
	}

	if (getPowerDomain() != pusher->getPowerDomain()) {
		return false;
	}

	if (getSegments() != pusher->getSegments()) {
		return false;
	}

	if (getPusherFlags() != pusher->getPusherFlags()) {
		return false;
	}

	return true;
}

bool PixelPusher::isAlive() {
	if ((std::clock() / CLOCKS_PER_SEC - mLastPingAt) < mTimeoutTime) {
		return true;
	}
	else {
		return false;
	}
}

void PixelPusher::createStrips() {
	for (int i = 0; i < mStripsAttached; i++) {
		std::shared_ptr<Strip> newStrip(new Strip(i, mPixelsPerStrip));
		mStrips.push_back(newStrip);
	}
}

void PixelPusher::createCardThread() {
	createStrips();

	mCardThreadSender = ofxAsio::udp::UdpSender::make();
	if (mLogLevel == DEBUG) {
		std::printf("PixelPusher::createCardThread -- Connected to PixelPusher %s on port %d\n", getIpAddress().c_str(), mPort);
	}
	mPacketNumber = 0;
	mThreadExtraDelay = 0;
	mCardThread = std::thread(&PixelPusher::sendPacket, this);
}

void PixelPusher::destroyCardThread() {
	mRunCardThread = false;
	if (mCardThread.joinable()) {
		mCardThread.join();
	}
}

void PixelPusher::setLogLevel(LogLevel log_level) {
	mLogLevel = log_level;
}

LogLevel PixelPusher::getLogLevel() {
	return mLogLevel;
}
