#include "mip_chain_fill.h"

MipChainFill::MipChainFill(alpha_img img) : _originalImage(img)
{
  _width = _originalImage.get_width();
  _height = _originalImage.get_height();
}

alpha_img MipChainFill::CompositeAlphaMip()
{
  mipmap vec = mipmap();

  png::uint_32 pow2min = std::min(_width, _height); // smallest dimension to reach 1 first

  int iterations = (int)log2(pow2min);
  vec.reserve(iterations);

  vec.push_back(_originalImage);

  for (size_t i = 0; i < iterations; ++i)
  {
    vec.push_back(AlphaMip(vec.back()));
  }

  for (int largeMapIdx = (int)vec.size() - 2, smallMapIdx = (int)vec.size() - 1;
    largeMapIdx >= 0;
    --largeMapIdx, --smallMapIdx)
  {

    auto larger = &vec[largeMapIdx];
    auto smaller = &vec[smallMapIdx];

    png::uint_32 largerWidth = larger->get_width();
    png::uint_32 largerHeight = larger->get_height();

    png::uint_32 smallerWidth = smaller->get_width();
    png::uint_32 smallerHeight = smaller->get_height();

    for (png::uint_32 y = 0; y < largerHeight; ++y)
    {
      for (png::uint_32 x = 0; x < largerWidth; ++x)
      {
        png::rgba_pixel pxl = larger->get_pixel(x, y);

        if (pxl.alpha < 255)
        {

          float wdelta = (float)x / (float)largerWidth;
          float hdelta = (float)y / (float)largerHeight;

          png::uint_32 xx = (png::uint_32)(wdelta * (float)smallerWidth);
          png::uint_32 yy = (png::uint_32)(hdelta * (float)smallerHeight);

          png::rgba_pixel ppxl = smaller->get_pixel(xx, yy);

          pxl = LerpPixel(ppxl, pxl, pxl.alpha / 255.0f);
          larger->set_pixel(x, y, ppxl);
        }
      }
    }
  }

  auto returnImg = vec.front();

  //apply original alpha
  for (png::uint_32 y = 0; y < _height; ++y)
  {
    for (png::uint_32 x = 0; x < _width; ++x)
    {
      returnImg[y][x].alpha = _originalImage[y][x].alpha;
    }
  }

  return returnImg;
}

alpha_img MipChainFill::AlphaMip(const alpha_img& input)
{
  const png::uint_32 original_width = input.get_width();
  const png::uint_32 original_height = input.get_height();

  const png::uint_32 width = original_width / 2;
  const png::uint_32 height = original_height / 2;

  alpha_img image(width, height);

  png::uint_32 originalX = 0;
  png::uint_32 originalY = 0;

  png::uint_32 north = 0;
  png::uint_32 east = 0;
  png::uint_32 south = 0;
  png::uint_32 west = 0;

  for (size_t y = 0; y < height; ++y, originalY += 2)
  {
    north = std::max((int)originalY - 1, 0);
    south = std::min(originalY + 1, original_height - 1);

    originalX = 0;

    for (size_t x = 0; x < width; ++x, originalX += 2)
    {
      east = std::min(originalX + 1, original_width - 1);
      west = std::max((int)originalX - 1, 0);

      png::rgba_pixel pxl = input.get_pixel(originalX, originalY);

      png::rgba_pixel pxlNorth = input.get_pixel(originalX, north);
      png::rgba_pixel pxlEast = input.get_pixel(east, originalY);
      png::rgba_pixel pxlSouth = input.get_pixel(originalX, south);
      png::rgba_pixel pxlWest = input.get_pixel(west, originalY);

      if (pxlNorth.alpha > pxl.alpha)
      {
        pxl = pxlNorth;
      }

      if (pxlEast.alpha > pxl.alpha)
      {
        pxl = pxlEast;
      }

      if (pxlSouth.alpha > pxl.alpha)
      {
        pxl = pxlSouth;
      }

      if (pxlWest.alpha > pxl.alpha)
      {
        pxl = pxlWest;
      }

      if (width == 1 || height == 1)
      {
        pxl.alpha = 255;
      }

      image.set_pixel(x, y, pxl);
    }
  }
  return image;
}
