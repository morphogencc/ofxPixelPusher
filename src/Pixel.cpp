#include "Pixel.h"

Pixel::Pixel() {
  mUseAntiLog = true;
  mRed = 0;
  mGreen = 0;
  mBlue = 0;
  mOrange = 0;
  mWhite = 0;
}

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) {
  mUseAntiLog = true;
  mRed = r;
  mGreen = g;
  mBlue = b;
  mOrange = 0;
  mWhite = 0;
}

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w) {
  mUseAntiLog = true;
  mRed = r;
  mGreen = g;
  mBlue = b;
  mOrange = o;
  mWhite = w;
}

Pixel::~Pixel() {
  
}

void Pixel::setColor(unsigned char r, unsigned char g, unsigned char b) {
  if(mUseAntiLog) {
    mRed = sLinearExp[r];
    mGreen = sLinearExp[g];
    mBlue = sLinearExp[b];
    mOrange = 0;
    mWhite = 0;
  }
  else {
    mRed = r;
    mGreen = g;
    mBlue = b;
    mOrange = 0;
    mWhite = 0;
  }
}

void Pixel::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w) {
  if(mUseAntiLog) {
    mRed = sLinearExp[r];
    mGreen = sLinearExp[g];
    mBlue = sLinearExp[b];
    mOrange = sLinearExp[o];
    mWhite = sLinearExp[w];
  }
  else {
    mRed = r;
    mGreen = g;
    mBlue = b;
    mOrange = o;
    mWhite = w;
  }
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
