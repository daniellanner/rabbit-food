#include "utilities.h"
#include "mip_chain_fill.h"

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

    MipChainFill fill = MipChainFill(image);

    png::image< png::rgba_pixel > output = fill.CompositeAlphaMip();

    output.write("resized.png");
  }
  catch (std::exception const& error)
  {
    std::cerr << "pixel_generator: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
}
