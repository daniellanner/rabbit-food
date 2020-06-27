#include <iostream>
#include <cstdlib>
#include <png.hpp>
#include <algorithm>
#include <fstream>

png::image<png::rgba_pixel> resize_image(png::image<png::rgba_pixel> input)
{

  const size_t original_width = input.get_width();
  const size_t original_height = input.get_height();

  const size_t width = original_width / 2;
  const size_t height = original_height / 2;

  png::image<png::rgba_pixel> image(width, height);
  
  size_t originalX = 0;
  size_t originalY = 0;

  for (size_t y = 0; y < height; ++y, originalY += 2)
  {
    originalY = std::min(originalY, original_height - 2);

    for (size_t x = 0, originalX = 0; x < width; ++x, originalX += 2)
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

      image.set_pixel(x, y, pxl);
    }
  }

  return image;
}

inline bool FileExists(const std::string& name) {
  std::ifstream f(name.c_str());
  return f.good();
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

    png::image< png::rgba_pixel > output = resize_image(image);
    output.write("resized.png");


    //size_t const width = image.get_width();
    //size_t const height = image.get_height();

    //for (size_t y = 0; y < height; ++y)
    //{
    //  for (size_t x = 0; x < width; ++x)
    //  {
    //    auto pxl = image.get_pixel(x, y);

    //    pxl.red = 1.0 - pxl.red;
    //    pxl.blue = 1.0 - pxl.blue;
    //    pxl.green = 1.0 - pxl.green;

    //    image.set_pixel(x, y, pxl);
    //  }
    //}

    //image.write("output.png");
  }
  catch (std::exception const& error)
  {
    std::cerr << "pixel_generator: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
}
