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
	mID = NULL;
	mStripType = STRIP;

	resetTexture();
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

void Strip::setPixelsFromTex() {
	if (mTexSrc != nullptr) {
		if (mUseTexCoords) {

			unsigned char* subTexSrc = new unsigned char[mSubTexWidth*mSubTexHeight*mTexDepth];

			if (mSubTexWidth * mSubTexHeight < mPixels.size()) {
				std::printf("Strip::setPixelsFromTex ERROR -- not enough pixels in subtexture to map to physical pixels.  Please set new texture coordinates.\n");
				return;
			}

			for (int y = 0; y < mSubTexHeight; y++) {
				for (int x = 0; x < mSubTexWidth; x++) {
					for (int d = 0; d < mTexDepth; d++) {
						if (y % 2 == 0) {
							// even row
							subTexSrc[y*mSubTexWidth*mTexDepth + x*mTexDepth + d] = mTexSrc[(mSubTexY + y)*mTexWidth*mTexDepth + (mSubTexX + x)*mTexDepth + d];
						}
						else {
							// even row
							subTexSrc[y*mSubTexWidth*mTexDepth + (mSubTexWidth - x)*mTexDepth + d] = mTexSrc[(mSubTexY + y)*mTexWidth*mTexDepth + (mSubTexX + x)*mTexDepth + d];
						}
					}
				}
			}

			scrapeTexture(subTexSrc, mSubTexWidth, mSubTexHeight, mTexDepth);

			//do I need to free(subTexSrc)? or delete?
			delete subTexSrc;
		}
		else {
			scrapeTexture(mTexSrc, mTexWidth, mTexHeight, mTexDepth);
		}
		mTouched = true;
	}

	else {
		std::printf("Strip::setPixelsFromTex ERROR -- no texture assigned!  Please assign a texture to this strip.\n");
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

std::vector<unsigned char>::iterator Strip::begin() {
	return mPixelData.begin();
}

std::vector<unsigned char>::iterator Strip::end() {
	return mPixelData.end();
}

void Strip::setID(int id) {
	mID = id;
}

int Strip::getID() {
	return mID;
}

void Strip::setStripType(StripType type) {
	if (type == StripType::TWENTYSQUARED && mPixels.size() < 400) {
		std::printf("Strip::setStriPType ERROR -- not enough pixels for this strip to be set as a TwentySquared tile.  Please check your PixelPusher's pixel.rc file.\n");
	}
	mStripType = type;
}

StripType Strip::getStripType() {
	return mStripType;
}

void Strip::setTexture(unsigned char* source, int width, int height, int depth) {
	if (width * height < mPixels.size()) {
		std::printf("Strip::setTexture ERROR -- not enough pixels in texture to map to physical pixels.  Please use a larger texture.\n");
		return;
	}
	mTexSrc = source;
	mTexWidth = width;
	mTexHeight = height;
	mTexDepth = depth;
}

void Strip::resetTexture() {
	mTexSrc = nullptr;
	mTexWidth = 0;
	mTexHeight = 0;
	mTexDepth = 0;
	mUseTexCoords = false;
	mSubTexX = 0.0;
	mSubTexY = 0.0;
	mSubTexWidth = 0.0;
	mSubTexHeight = 0.0;
}

void Strip::setTexCoords(float lower_x, float lower_y, float upper_x, float upper_y) {
	if (lower_x > upper_x || lower_y > upper_y) {
		std::printf("Strip::setTexCoords ERROR -- lower bounds greater than upper bounds.\n");
		return;
	}
	if (lower_x < 0 || upper_x < 0 || lower_y < 0 || upper_y < 0) {
		std::printf("Strip::setTexCoords ERROR -- texture coordinates must be greater than zero.\n");
		return;
	}

	mSubTexX = lower_x*mTexWidth;
	mSubTexY = lower_y*mTexHeight;
	mSubTexWidth = (upper_x - lower_x)*mTexWidth + 1;
	mSubTexHeight = (upper_y - lower_y)*mTexHeight + 1;

	if (mSubTexWidth*mSubTexHeight < mPixels.size()) {
		std::printf("Strip::setTexCoords ERROR -- not enough pixels in subtexture to map to physical pixels.  Please set new texture coordinates.\n");
		return;
	}

	mUseTexCoords = true;
}

void Strip::scrapeTexture(unsigned char* source, int width, int height, int depth) {
	int stride_x;
	int stride_y;

	switch(mStripType) {
	case STRIP:
		//evenly distribute pixels along length of strip
		stride_x = (width*height) / (float)mPixels.size();
		for (int i = 0; i < mPixels.size(); i++) {
			mPixels[i]->setColor(source[stride_x*i*depth + 0], source[stride_x*i*depth + 1], source[stride_x*i*depth + 2]);
		}
		break;
	case TWENTYSQUARED:
		//evenly distribute pixels across x and y
		stride_x = int(width / 20.0);
		stride_y = int(height / 20.0);
		for (int j = 0; j < 20; j++) {
			for (int i = 0; i < 20; i++) {
				if (j % 2 == 0) {
					//even row
					int index = i + 20 * j;
					int source_pixel = i*stride_x + j*stride_y*width;
					setPixel(index, source[source_pixel*depth + 0], source[source_pixel*depth + 1], source[source_pixel*depth + 2]);
				}
				else {
					//odd row
					// FIX THIS
					int index = 20 * (j+1) - (i+1);
					int source_pixel = i*stride_x + j*stride_y*width;
					setPixel(index, source[source_pixel*depth + 0], source[source_pixel*depth + 1], source[source_pixel*depth + 2]);
				}
			}
		}
		break;
	}
	
}
