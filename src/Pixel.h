class Pixel {
 public:
  byte red, green, blue;
  Pixel();
  Pixel(Pixel pixel);
  Pixel(byte red, byte green, byte blue);
  ~Pixel();
  void setColor(int color);
  void setColor(Pixel pixel);
}
