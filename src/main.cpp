#include <iostream>
#include <cstdlib>
#include <png.hpp>
#include <algorithm>
#include <fstream>
#include <cmath>

typedef std::vector<png::image<png::rgba_pixel>> mipmap;
typedef png::image<png::rgba_pixel> alpha_img;

constexpr auto EPSILON = 0.00001;

inline float FastLerp(const float a, const float b, const float t)
{
  return a + t * (b - a);
}

png::rgba_pixel LerpPixel(png::rgba_pixel a, png::rgba_pixel b, float t)
{
  float red = FastLerp(a.red, b.red, t);
  float green = FastLerp(a.green, b.green, t);
  float blue = FastLerp(a.blue, b.blue, t);

  return png::rgba_pixel(red, green, blue, 255);
}

alpha_img AlphaMip(const alpha_img& input)
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


alpha_img CompositeAlphaMip(alpha_img input)
{
  mipmap map = mipmap();

  png::uint_32 width = input.get_width();
  png::uint_32 height = input.get_height();
  png::uint_32 pow2min = std::min(width, height); // smallest dimension to reach 1 first

  int iterations = log2(pow2min);
  map.reserve(iterations);

  map.push_back(input);

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
        png::rgba_pixel pxl = larger->get_pixel(x,y);

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

    //larger->write(std::to_string(largeMapIdx) + ".png");
  }

  return map.front();
}


void Resize(alpha_img& input, const png::uint_32 width, const png::uint_32 height)
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

int main(int argc, char** argv)
{
  std::cout << "Hello World!\n";

  if (argc < 2)
  {
    // no input file selected
    return 0;
  }

  std::cout << "You have entered " << argc
    << " arguments:" << "\n";

  for (int i = 0; i < argc; ++i)
  {
    std::cout << argv[i] << "\n";
  }

  std::string originPath = argv[0];
  std::string inputFile = argv[1];

  if (!FileExists(inputFile))
  {
    std::cout << "ERROR: Input file does not exist at location " << inputFile << std::endl;
    return 1;
  }

  try
  {
    png::image< png::rgba_pixel > image(inputFile);

    png::uint_32 originalWidth =  image.get_width();
    png::uint_32 originalHeight = image.get_height();

    png::uint_32 pow2Width = originalWidth;
    png::uint_32 pow2Height = originalHeight;

    if (!IsPowerOfTwo(originalWidth))
    {
      std::cout <<
        "WARNING: width of image is not power of 2. Attempting resize.\n"
        "Should the output not be as expected please manually resize to power of 2."
        << std::endl;

      pow2Width = CalculatePowerOfTwo(originalWidth);
    }

    if (!IsPowerOfTwo(originalHeight))
    {
      std::cout <<
        "WARNING: height of image is not power of 2. Attempting resize.\n"
        "Should the output not be as expected please manually resize to power of 2."
        << std::endl;

      pow2Height = CalculatePowerOfTwo(originalHeight);
    }

    
    Resize(image, pow2Width, pow2Height);
    png::image< png::rgba_pixel > output = CompositeAlphaMip(image);

    //apply original alpha
    //for (png::uint_32 y = 0; y < image.get_height(); ++y)
    //{
    //  for (png::uint_32 x = 0; x < image.get_width(); ++x)
    //  {
    //    output[y][x].alpha = image[y][x].alpha;
    //  }
    //}

    output.write("resized.png");
  }
  catch (std::exception const& error)
  {
    std::cerr << "pixel_generator: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
}
