#include "Pixel.h"
#include "Utilities.h"

using namespace ofxPixelPusher;

unsigned char Pixel::mLinearExp[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12,
13, 13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27,
27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 54, 55, 56, 57,
59, 60, 61, 63, 64, 65, 67, 68, 70, 72, 73, 75, 76, 78, 80, 82, 83, 85, 87, 89, 91, 93, 95, 97, 99, 102, 104, 106, 109, 111, 114, 116,
119, 121, 124, 127, 129, 132, 135, 138, 141, 144, 148, 151, 154, 158,
161, 165, 168, 172, 176, 180, 184, 188, 192, 196, 201, 205,
209, 214, 219, 224, 229, 234, 239, 244, 249, 255 };

Pixel::Pixel() {
	mUseAntiLog = true;
	mRed = 0;
	mGreen = 0;
	mBlue = 0;
	mOrange = 0;
	mWhite = 0;
	setColorCorrection(ColorCorrection::UNCORRECTED);
	setColorTemperature(ColorTemperature::UNCORRECTED_TEMPERATURE);
}

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) {
	mUseAntiLog = true;
	mRed = r;
	mGreen = g;
	mBlue = b;
	mOrange = 0;
	mWhite = 0;
	setColorCorrection(ColorCorrection::UNCORRECTED);
	setColorTemperature(ColorTemperature::UNCORRECTED_TEMPERATURE);
}

Pixel::~Pixel() {

}

unsigned char Pixel::getRed() {
	return mRed;
}

unsigned char Pixel::getGreen() {
	return mGreen;
}

unsigned char Pixel::getBlue() {
	return mBlue;
}

ColorRGB Pixel::getColor() {
	ColorRGB color;
	color.mRed = mRed;
	color.mGreen = mGreen;
	color.mBlue = mBlue;
	return color;
}

void Pixel::setColor(unsigned char r, unsigned char g, unsigned char b) {
	if (mUseAntiLog) {
		mRed = mCorrection[0] * mLinearExp[r];
		mGreen = mCorrection[1] * mLinearExp[g];
		mBlue = mCorrection[2] * mLinearExp[b];
		mOrange = 0;
		mWhite = 0;
	}
	else {
		mRed = mCorrection[0] * r;
		mGreen = mCorrection[1] * g;
		mBlue = mCorrection[2] * b;
		mOrange = 0;
		mWhite = 0;
	}
}

void Pixel::setColorHSB(float hue, float saturation, float brightness) {
	ColorRGB rgbColor = hsb2rgb(hue, saturation, brightness);
	setColor(rgbColor.mRed, rgbColor.mGreen, rgbColor.mBlue);
}

void Pixel::setColor(Pixel pixel) {
	mRed = pixel.mRed;
	mGreen = pixel.mGreen;
	mBlue = pixel.mBlue;
	mOrange = pixel.mOrange;
	mWhite = pixel.mWhite;
}

void Pixel::setAntiLog(bool useAntiLog) {
	mUseAntiLog = useAntiLog;
}

void Pixel::setManualCorrection(uint32_t correction) {
	mCorrection[0] = float((((correction >> 16) & 0xFF)) * (mCorrection[0])) / 65535;
	mCorrection[1] = float((((correction >> 8) & 0xFF)) * (mCorrection[1])) / 65535;
	mCorrection[2] = float((((correction >> 0) & 0xFF)) * (mCorrection[2])) / 65535;
}


void Pixel::setColorCorrection(ColorCorrection correction) {
	mColorCorrection = correction;
	calculateCorrection();
}

void Pixel::setColorTemperature(ColorTemperature temperature) {
	mColorTemperature = temperature;
	calculateCorrection();
}

void Pixel::calculateCorrection() {
	mCorrection[0] = float((((mColorCorrection >> 16) & 0xFF)) * (((mColorTemperature >> 16) & 0xFF))) / 65535;
	mCorrection[1] = float((((mColorCorrection >> 8) & 0xFF)) * (((mColorTemperature >> 8) & 0xFF))) / 65535;
	mCorrection[2] = float((((mColorCorrection >> 0) & 0xFF)) * (((mColorTemperature >> 0) & 0xFF))) / 65535;
}