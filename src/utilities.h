#pragma once

#include <iostream>
#include <cstdlib>
#include <png.hpp>
#include <algorithm>
#include <fstream>

typedef std::vector<png::image<png::rgba_pixel>> mipmap;
typedef png::image<png::rgba_pixel> alpha_img;

constexpr auto EPSILON = 0.00001;

inline bool StringCompare(const std::string& str1, const std::string& str2)
{
  if (str1.size() != str2.size())
  {
    return false;
  }

  return std::equal(str1.begin(), str1.end(), str2.begin(),
      [](char& c1, char& c2)
      {
        return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
      }
  );
}

inline float FastLerp(const float a, const float b, const float t)
{
  return a + t * (b - a);
}

inline png::rgba_pixel LerpPixel(png::rgba_pixel a, png::rgba_pixel b, float t)
{
  float red = FastLerp(a.red, b.red, t);
  float green = FastLerp(a.green, b.green, t);
  float blue = FastLerp(a.blue, b.blue, t);

  return png::rgba_pixel(red, green, blue, 255);
}

inline void Resize(alpha_img& input, const png::uint_32 width, const png::uint_32 height)
{
  input.resize(width, height);
}

inline bool FileExists(const std::string& name) {
  std::ifstream f(name.c_str());
  return f.good();
}

// https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
inline bool IsPowerOfTwo(const png::uint_32 x)
{
  return (x != 0) && ((x & (x - 1)) == 0);
}


enum class NonPowerOfTwoResize {
  NEXT_SMALLEST,
  NEAREST,
  NEXT_LARGEST
};

inline png::uint_32 CalculatePowerOfTwo(const png::uint_32 x, NonPowerOfTwoResize option = NonPowerOfTwoResize::NEAREST)
{
  png::uint_32 xpow;

  switch (option)
  {
  case NonPowerOfTwoResize::NEXT_SMALLEST:
  default:
  {
    xpow = floor(log2(x));
    break;
  }
  case NonPowerOfTwoResize::NEAREST:
  {
    xpow = round(log2(x));
    break;
  }
  case NonPowerOfTwoResize::NEXT_LARGEST:
  {
    xpow = ceil(log2(x));
    break;
  }
  }

  return (png::uint_32)pow(2, xpow);
}
