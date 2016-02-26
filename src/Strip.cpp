#include "Strip.h"

using namespace ofxPixelPusher;

Strip::Strip(short stripNumber, int length) {
	mPixels.reserve(length);
	for (int i = 0; i < length; i++) {
		mPixels.push_back(std::shared_ptr<Pixel>(new Pixel()));
	}

	mPixelData.reserve(3 * length);
	for (int i = 0; i < 3 * length; i++) {
		mPixelData.push_back(0);
	}

	mStripNumber = stripNumber;
	mTouched = false;
	mIsRGBOW = false;

	mPowerScale = 1.0;
}

Strip::~Strip() {
}

int Strip::getLength() {
	return mPixels.size();
}

bool Strip::isTouched() {
	return mTouched;
}

short Strip::getStripNumber() {
	return mStripNumber;
}

void Strip::setPixels(unsigned char r, unsigned char g, unsigned char b) {
	for (int i = 0; i < mPixels.size(); i++) {
		mPixels[i]->setColor(r, g, b);
	}
	mTouched = true;
}

void Strip::setPixelsHSB(float hue, float saturation, float brightness) {
	for (int i = 0; i < mPixels.size(); i++) {
		mPixels[i]->setColorHSB(hue, saturation, brightness);
	}
	mTouched = true;
}

void Strip::setPixels(std::vector<std::shared_ptr<Pixel> > pixels) {
	mPixels = pixels;
	mTouched = true;
}

void Strip::setPixel(int position, unsigned char r, unsigned char g, unsigned char b) {
	if (position < mPixels.size()) {
		mPixels[position]->setColor(r, g, b);
		mTouched = true;
	}
	else {
		std::printf("Strip::setPixel ERROR -- Invalid pixel number %d.\n", position);
	}
}

void Strip::setPixelHSB(int position, float hue, float saturation, float brightness) {
	if (position < mPixels.size()) {
		mPixels[position]->setColorHSB(hue, saturation, brightness);
		mTouched = true;
	}
	else {
		std::printf("Strip::setPixelHSB ERROR -- Invalid pixel number %d.\n", position);
	}
}

void Strip::setPixel(int position, std::shared_ptr<Pixel> pixel) {
	if (position < mPixels.size()) {
		mPixels[position] = pixel;
		mTouched = true;
	}
	else {
		std::printf("Strip::setPixel ERROR -- Invalid pixel number %d.\n", position);
	}
}

std::vector<std::shared_ptr<Pixel> > Strip::getPixels() {
	return mPixels;
}

int Strip::getNumberOfPixels() {
	return mPixels.size();
}

void Strip::setPowerScale(double powerscale) {
	if (powerscale < 1.0 && powerscale > 0.0) {
		mPowerScale = powerscale;
	}
	else {
		std::printf("Strip::setPowerScale -- power scale set out of bounds (0, 1).  Please set the power scale between 0 and 1!\n");
	}
}

void Strip::serialize() {
	for (int i = 0; i < mPixels.size(); i++) {
		mPixelData[3 * i + 0] = (unsigned char)(mPixels[i]->mRed * mPowerScale);
		mPixelData[3 * i + 1] = (unsigned char)(mPixels[i]->mGreen * mPowerScale);
		mPixelData[3 * i + 2] = (unsigned char)(mPixels[i]->mBlue * mPowerScale);
	}
	mTouched = false;
}

unsigned char* Strip::getPixelData() {
	return mPixelData.data();
}

int Strip::getPixelDataLength() {
	return mPixelData.size();
}

std::vector<unsigned char>::iterator Strip::begin() {
	return mPixelData.begin();
}

std::vector<unsigned char>::iterator Strip::end() {
	return mPixelData.end();
}
