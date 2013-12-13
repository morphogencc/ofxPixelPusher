class Strip {
 public:
  Strip(PixelPusher pusher, int stripNumber, int length);
  int getLength();
  void setPowerScale(double scale);
  String getMacAddress();
  bool isTouched();
  int getStripNumber();
  long getStripIdentifier();
  void setPixels(Pixel* pixels); //need to be vector-ized
  void setPixel(int color, int position);
  void setPixel(Pixel pixel, int position);
  byte serialize(); //returns byte array
  
 private:
  std::vector<Pixel> pixels;
  PixelPusher pusher;
  int stripNumber;
  bool touched;
  double powerScale;
