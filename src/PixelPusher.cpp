#include "PixelPusher.h"
#include <algorithm>
#include <ctime>
#include <iterator>
#include "ofMain.h"

using namespace ofxPixelPusher;

//PixelPusher::PixelPusher(DeviceHeader* header) {
PixelPusher::PixelPusher(std::shared_ptr<DeviceHeader> header) {
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
    mPusherFlags = 0;
	mPacketNumber = 0;
	mThreadDelay = 0;
	mThreadExtraDelay = 0;
	mTotalDelay = 0;
	mRunCardThread = NULL;    

	mMulticast = false;
	mMulticastPrimary = false;
	mAutothrottle = false;
	mLastPingAt = std::clock() / CLOCKS_PER_SEC;
	mResetSentAt = std::clock() / CLOCKS_PER_SEC;
	mSendReset = false;

    mMacAddress = header->getMacAddressString();
    mIpAddress = header->getIpAddressString();
    mSoftwareVersion = header->getSoftwareRevision()/ 100.0f;
    mHardwareVersion = header->getHardwareRevision();

    unsigned char* packetRemainder = header->getPacketRemainder();
	int packetLength = header->getPacketRemainderLength();

	if (packetLength < 28) {
        ofLogWarning() << "Packet size is too small! PixelPusher can't be created.";
	}

    memcpy(&mStripsAttached, &packetRemainder[0], 1);
    memcpy(&mMaxStripsPerPacket, &packetRemainder[1], 1);
    memcpy(&mPixelsPerStrip, &packetRemainder[2], 2);
    memcpy(&mUpdatePeriod, &packetRemainder[4], 4);
    memcpy(&mPowerTotal, &packetRemainder[8], 4);
    memcpy(&mDeltaSequence, &packetRemainder[12], 4);
    memcpy(&mControllerId, &packetRemainder[16], 4);
    memcpy(&mGroupId, &packetRemainder[20], 4);
    memcpy(&mArtnetUniverse, &packetRemainder[24], 2);
    memcpy(&mArtnetChannel, &packetRemainder[26], 2);

	if (packetLength < 28 && header->getSoftwareRevision() > 100) {
        memcpy(&mPort, &packetRemainder[28], 2);
	}
	else {
		mPort = 9897;
	}
	short defaultNumberOfStrips = 8;
	short stripFlagSize = std::max(mStripsAttached, defaultNumberOfStrips);
	mStripFlags.resize(stripFlagSize);

	if (packetLength > 30 && header->getSoftwareRevision() > 108) {
        memcpy(&mStripFlags[0], &packetRemainder[30], stripFlagSize);
	}
	else {
		for (int i = 0; i < stripFlagSize; i++) {
			mStripFlags[i] = 0;
		}
	}

	if (packetLength > 30 + stripFlagSize && header->getSoftwareRevision() > 108) {
		// set Pusher flags
		long pusherFlags;
        memcpy(&pusherFlags, &packetRemainder[32 + stripFlagSize], 4);
		setPusherFlags(pusherFlags);
        memcpy(&mSegments, &packetRemainder[36 + stripFlagSize], 4);
        memcpy(&mPowerDomain, &packetRemainder[40 + stripFlagSize], 4);
	}

	mPacket.reserve(400 * mMaxStripsPerPacket); //too high, can be reduced
}

PixelPusher::~PixelPusher() {
    if(mCardThread.joinable()) {
        mRunCardThread = false;
        mCardThread.join();
         ofLogVerbose() << "Destruct Pixel Pusher: Card Thread Joined";
    }
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
        ofLogError("PixelPusher") << "getStrip ERROR -- Invalid strip number " << stripNumber << ". Returning empty strip.";
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
        ofLogError("PixelPusher") << "getPixelsPerStrip ERROR -- Invalid strip number " << stripNumber << ".";
		return 0;
	}
}

void PixelPusher::setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue) {
	if (stripNumber < mStrips.size()) {
		mStrips.at(stripNumber)->setPixels(red, green, blue);
	}
	else {
        ofLogError("PixelPusher") << "setStripValues ERROR -- Invalid strip number " << stripNumber << ".";
	}
}

void PixelPusher::setStripValues(int stripNumber, std::vector<std::shared_ptr<Pixel> > pixels) {
	if (stripNumber < mStrips.size()) {
		mStrips.at(stripNumber)->setPixels(pixels);
	}
	else {
        ofLogError("PixelPusher") << "setStripValues ERROR -- Invalid strip number " << stripNumber << ".";
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
        ofLogError("PixelPusher") << "setPowerScale ERROR -- Invalid strip number " << stripNumber << ".";
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
    return mMacAddress;
}

std::string PixelPusher::getIpAddress() {
    return mIpAddress;
}


float PixelPusher::getSoftwareVersion() {
    return mSoftwareVersion;
}

short PixelPusher::getHardwareVersion() {
    return mHardwareVersion;
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

        //ofLogVerbose("PixelPusher") << "Updating total delay for PixelPusher " << getMacAddress().c_str() << " to " << mTotalDelay;

		if (!mSendReset && remainingStrips.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(mTotalDelay));
		}

		while (!remainingStrips.empty()) {
            ofLogVerbose("PixelPusher") << "Sending data to PixelPusher " << getMacAddress().c_str() << " at " << getIpAddress().c_str() << ":" <<  mPort;
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
                ofLogVerbose("PixelPusher") << "Payload confirmed; sending packet of " << mPacket.size() << " bytes";
				mPacketNumber++;

				std::shared_ptr<ofxAsio::Datagram> packet = std::make_shared<ofxAsio::Datagram>(mPacket, getIpAddress(), getPort());
				mCardThreadSender->send(packet);
				payload = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(mTotalDelay));
			}
		}
	}

    ofLogVerbose("PixelPusher") << "Closing Card Thread for PixelPusher " << getMacAddress().c_str();
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

    if (mPusherFlags != pusher->getPusherFlags()) {
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
    ofLogNotice("PixelPusher") << "Connected to PixelPusher " << getIpAddress().c_str() << " on port " << mPort;
	mPacketNumber = 0;
	mThreadExtraDelay = 0;
	mCardThread = std::thread(&PixelPusher::sendPacket, this);
}

void PixelPusher::destroyCardThread() {
    ofLogVerbose("PixelPusher") << "Destroy card thread!";
	mRunCardThread = false;
	if (mCardThread.joinable()) {
		mCardThread.join();
	}
}
