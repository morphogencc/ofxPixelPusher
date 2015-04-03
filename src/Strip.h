#pragma once

#ifdef _WIN32
#include <memory>
#elif __APPLE__
#include <tr1/memory>
#endif

#include <vector>
#include <string>
#include "Pixel.h"

using namespace std::tr1;

class Strip {
 public:
  Strip(short stripNumber, int length);
  ~Strip();
  bool isRGBOW();
  void setRGBOW(bool rgbow);
  int getLength();
  bool isTouched();
  short getStripNumber();
  void setPixels(unsigned char r, unsigned char g, unsigned char b);
  //void setPixels(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
  void setPixels(std::vector<shared_ptr<Pixel> > p);
  void setPixel(int position, unsigned char r, unsigned char g, unsigned char b);
  //void setPixel(int position, unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
  void setPixel(int position, shared_ptr<Pixel> pixel);
  std::vector<shared_ptr<Pixel> > getPixels();
  int getNumPixels();
  void setPowerScale(double powerscale);
  void serialize();
  unsigned char* getPixelData(); //remove
  int getPixelDataLength(); //remove
  std::vector<unsigned char>::iterator begin();
  std::vector<unsigned char>::iterator end();
 protected:
  std::vector<shared_ptr<Pixel> > mPixels;
  std::vector<unsigned char> mPixelData;
  short mStripNumber;
  bool mTouched;
  bool mIsRGBOW;
  double mPowerScale;
};
