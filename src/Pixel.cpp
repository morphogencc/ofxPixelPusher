#include "Pixel.h"

Pixel::Pixel() {
  red = 0;
  green = 0;
  blue = 0;
}

Pixel::Pixel(Pixel pixel) {
  /*
   * Is there a need for a copy constructor? doesn't c++ do this automatically?
   */
  red = pixel.red;
  green = pixel.green;
  blue = pixel.blue;
}

Pixel::Pixel(byte _red, byte _green, byte _blue) {
  red = _red;
  green = _red;
  blue = _blue;
}
  
}
  
}
