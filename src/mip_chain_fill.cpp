#include "mip_chain_fill.h"

MipChainFill::MipChainFill(alpha_img img) : _originalImage(img)
{
  _width = _originalImage.get_width();
  _height = _originalImage.get_height();
}

alpha_img MipChainFill::CompositeAlphaMip()
{
  mipmap map = mipmap();

  png::uint_32 pow2min = std::min(_width, _height); // smallest dimension to reach 1 first

  int iterations = log2(pow2min);
  map.reserve(iterations);

  map.push_back(_originalImage);

  for (size_t i = 0; i < iterations; ++i)
  {
    map.push_back(AlphaMip(map.back()));
  }

  for (int largeMapIdx = map.size() - 2, smallMapIdx = map.size() - 1;
    largeMapIdx >= 0;
    --largeMapIdx, --smallMapIdx)
  {

    auto larger = &map[largeMapIdx];
    auto smaller = &map[smallMapIdx];

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

          pxl = LerpPixel(ppxl, pxl, pxl.alpha / 255.0);
          larger->set_pixel(x, y, ppxl);
        }
      }
    }
  }

  auto returnImg = map.front();

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

  for (size_t y = 0; y < height; ++y, originalY += 2)
  {
    originalY = std::min(originalY, original_height - 2);
    originalX = 0;

    for (size_t x = 0; x < width; ++x, originalX += 2)
    {
      originalX = std::min(originalX, original_width - 2);

      png::rgba_pixel pxl;

      png::rgba_pixel pxl1 = input.get_pixel(originalX, originalY);
      png::rgba_pixel pxl2 = input.get_pixel(originalX + 1, originalY);
      png::rgba_pixel pxl3 = input.get_pixel(originalX, originalY + 1);
      png::rgba_pixel pxl4 = input.get_pixel(originalX + 1, originalY + 1);


      pxl = pxl1;

      if (pxl2.alpha > pxl.alpha)
      {
        pxl = pxl2;
      }

      if (pxl3.alpha > pxl.alpha)
      {
        pxl = pxl3;
      }

      if (pxl4.alpha > pxl.alpha)
      {
        pxl = pxl4;
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
