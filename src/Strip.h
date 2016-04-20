/*
* PixelPusher
* Dec 2014
* nathan lachenmyer
* http://morphogen.cc/
*/

#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Pixel.h"

namespace ofxPixelPusher {

	class Strip {
	public:
		Strip(short stripNumber, int length);
		~Strip();
		//bool isRGBOW();
		//void setRGBOW(bool rgbow);
		int getLength();
		bool isTouched();
		short getStripNumber();
		void setPixels(unsigned char r, unsigned char g, unsigned char b);
		void setPixelsHSB(float hue, float saturation, float brightness);
		//void setPixels(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
		void setPixels(std::vector<std::shared_ptr<Pixel> > p);
		void setPixel(int position, unsigned char r, unsigned char g, unsigned char b);
		void setPixelHSB(int position, float hue, float saturation, float brightness);
		//void setPixel(int position, unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
		void setPixel(int position, std::shared_ptr<Pixel> pixel);
		void setPixelsFromTex();
		void setTexture(unsigned char* source, int width, int height, int depth);
		void resetTexture();
		void setTexCoords(float lower_x, float lower_y, float upper_x, float upper_y);
		std::vector<std::shared_ptr<Pixel> > getPixels();
		int getNumberOfPixels();
		void setPowerScale(double powerscale);
		void serialize();
		void setID(int id);
		int getID();
		std::vector<unsigned char>::iterator begin();
		std::vector<unsigned char>::iterator end();
	protected:
		void scrapeTexture(unsigned char* source, int width, int height, int depth);
		std::vector<std::shared_ptr<Pixel> > mPixels;
		std::vector<unsigned char> mPixelData;
		short mStripNumber;
		bool mTouched;
		bool mIsRGBOW;
		double mPowerScale;
		int mID;
		int mTexWidth;
		int mTexHeight;
		int mTexDepth;
		unsigned char* mTexSrc;
		int mSubTexY;
		int mSubTexX;
		int mSubTexWidth;
		int mSubTexHeight;
		bool mUseTexCoords;
	};

}
