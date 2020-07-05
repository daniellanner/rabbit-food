#pragma once

#include <iostream>
#include <cstdlib>
#include <png.hpp>
#include <algorithm>
#include <fstream>

#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define TODO(_message) __pragma(message(__FILE__ "(" $Line ") : TODO : " _message))

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
      [](const char& c1, const char& c2)
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

  return png::rgba_pixel((png::byte)red, (png::byte)green, (png::byte)blue, 255);
}

/// <summary>
/// Naive resize based on unweighted nearest neighbour. It really is preferred to enter a power of 2 texture.
/// Note for big textures: Allocates power of two image on stack, then deep copies to original image.
/// </summary>
/// <param name="input">Input texture per ref. will be overwritten.</param>
/// <param name="width">Width to resize to.</param>
/// <param name="height">Height to resize to.</param>
inline void Resize(alpha_img& input, const png::uint_32 width, const png::uint_32 height)
{
  const png::uint_32 oldWidth = input.get_width();
  const png::uint_32 oldHeight = input.get_height();

  alpha_img resized(width, height);

  for (png::uint_32 y = 0; y < height; ++y)
  {
    float yfactor = (float)y / (float)height;
    png::uint_32 yidx = (png::uint_32)(yfactor * oldHeight);

    for (png::uint_32 x = 0; x < width; ++x)
    {
      float xfactor = (float)x / (float)width;
      png::uint_32 xidx = (png::uint_32)(xfactor * oldWidth);

      resized[y][x] = input[yidx][xidx];
    }
  }
  input.resize(0, 0);
  input = resized;
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

  TODO("check floating point errors with extreme values")
  switch (option)
  {
  case NonPowerOfTwoResize::NEXT_SMALLEST:
  default:
  {
    xpow = (png::uint_32)floor(log2(x));
    break;
  }
  case NonPowerOfTwoResize::NEAREST:
  {
    xpow = (png::uint_32)round(log2(x));
    break;
  }
  case NonPowerOfTwoResize::NEXT_LARGEST:
  {
    xpow = (png::uint_32)ceil(log2(x));
    break;
  }
  }

  return (png::uint_32)pow(2, xpow);
}
