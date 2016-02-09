/*
 * Pixel
 * Dec 2014
 * nathan lachenmyer
 * http://morphogen.cc
 */

#pragma once

//forward declare Strip so I don't get a Pixel<->Strip interdependency
namespace ofxPixelPusher {

class Strip;

class Pixel {
  friend class Strip;
 public:
  Pixel();
  Pixel(unsigned char r, unsigned char g, unsigned char b);
  Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
  ~Pixel();
  void setColor(unsigned char r, unsigned char g, unsigned char b);
  void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
  void setColor (Pixel pixel);
  void setAntiLog(bool useAntiLog);
 protected:
  bool mUseAntiLog;
  unsigned char mRed;
  unsigned char mGreen;
  unsigned char mBlue;
  unsigned char mOrange;
  unsigned char mWhite;
  static unsigned char mLinearExp[256];
  
};

}