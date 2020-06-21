#include <iostream>
#include <cstdlib>
#include <png.hpp>

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

  
  try
  {
    png::image< png::rgb_pixel > image("output.png");

    size_t const width = image.get_width();
    size_t const height = image.get_height();

    for (size_t y = 0; y < height; ++y)
    {
      for (size_t x = 0; x < width; ++x)
      {
        auto pxl = image[x][y];

        pxl.red = 1.0 - pxl.red;
        pxl.blue = 1.0 - pxl.blue;
        pxl.green = 1.0 - pxl.green;

        image[x][y] = pxl;
      }
    }

    image.write("output.png");
  }
  catch (std::exception const& error)
  {
    std::cerr << "pixel_generator: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
}
