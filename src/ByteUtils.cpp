#include "ByteUtils.h"

namespace ByteUtils {
  short unsignedCharToShort(unsigned char c[], int start, int stop) {
    if( (stop - start) != 1) {
      //should probably make this an exception
      std::cout << "Bounds not properly sized for a short!" << std::endl;
    }
    short s = 0;
    s |= c[start] & 0xff;
    s <<= 8;
    s |= c[stop] & 0xff;
    s = ntohs(s);
    return s;
  }

  long unsignedCharToLong(unsigned char c[], int start, int stop) {
    if( (stop - start) != 3) {
      //should probably make this an exception
      std::cout << "Bounds not properly sized for a long!" << std::endl;
    }
    long l = 0;
    l |= c[start] & 0xff;
    l <<= 8;
    l |= c[start + 1] & 0xff;
    l <<= 8;
    l |= c[start + 2] & 0xff;
    l <<= 8;
    l |= c[start + 3] & 0xff;
    l = ntohl(l);
    return l;
  }

}
  
