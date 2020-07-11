#include "utilities.h"
#include "mip_chain_fill.h"
#include "cxxopts.hpp"

#define QUIT_SUCCESS 0
#define QUIT_ERROR_INPUT 1
#define QUIT_ERROR_OTHER 2

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    return QUIT_ERROR_INPUT;
  }

  TODO("try catch block with proper error handling")
  cxxopts::Options options("test", "A brief description");
  options.positional_help("<input file> <output file (optional)>");

  TODO("make help more readable")
  options.add_options()
    ("r,resize", "Resize non power of two -r larger, -r smaller, -r nearest", cxxopts::value<std::string>()->default_value("nearest"))
    ("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))
    ("h,help", "Print usage")
    ("i,input", "Input file", cxxopts::value<std::string>())
    ("o,output", "Output file", cxxopts::value<std::string>())
    ("positional", "Positional parameters", cxxopts::value<std::vector<std::string>>());
    
  options.parse_positional({ "input", "output", "positional" });
  auto result = options.parse(argc, argv);

  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    return QUIT_SUCCESS;
  }
  
  bool debug;
  std::string resize;
  int threads;

  if (result.count("debug"))
    debug = result["debug"].as<bool>();

  if (result.count("resize"))
    resize = result["resize"].as<std::string>();

  std::cout << "Resize: " << resize << std::endl;
  std::cout << "Debug: " << debug << std::endl;

  std::string inputpath = "";
  std::string outputpath = "";

  if (result.count("input"))
    inputpath = result["input"].as<std::string>();

  if (result.count("output"))
    outputpath = result["output"].as<std::string>();
  
  std::cout << "Input: " << inputpath << std::endl;
  std::cout << "Output: " << outputpath << std::endl;

  if (!FileExists(inputpath))
  {
    std::cout << "ERROR: Input file does not exist at location " << inputpath << std::endl;
    return QUIT_ERROR_INPUT;
  }

  try
  {
    png::image< png::rgba_pixel > image(inputpath);

    png::uint_32 originalWidth =  image.get_width();
    png::uint_32 originalHeight = image.get_height();

    png::uint_32 pow2Width = originalWidth;
    png::uint_32 pow2Height = originalHeight;

    NonPowerOfTwoResize resizeOption = NonPowerOfTwoResize::NEAREST;

    if (StringCompare(resize, "larger"))
    {
      resizeOption = NonPowerOfTwoResize::NEXT_LARGEST;
    }
    else if (StringCompare(resize, "smaller"))
    {
      resizeOption = NonPowerOfTwoResize::NEXT_SMALLEST;
    }

    if (!IsPowerOfTwo(originalWidth))
    {
      std::cout <<
        "WARNING: width of image is not power of 2. Attempting resize.\n"
        "Should the output not be as expected please manually resize to power of 2."
        << std::endl;

      pow2Width = CalculatePowerOfTwo(originalWidth, resizeOption);
    }

    if (!IsPowerOfTwo(originalHeight))
    {
      std::cout <<
        "WARNING: height of image is not power of 2. Attempting resize.\n"
        "Should the output not be as expected please manually resize to power of 2."
        << std::endl;

      pow2Height = CalculatePowerOfTwo(originalHeight, resizeOption);
    }

    Resize(image, pow2Width, pow2Height);

    MipChainFill fill = MipChainFill(image);

    png::image< png::rgba_pixel > output = fill.CompositeAlphaMip();

    TODO("check for set outputpath")
    output.write(outputpath);
  }
  catch (std::exception const& error)
  {
    std::cerr << "pixel_generator: " << error.what() << std::endl;
    return QUIT_ERROR_OTHER;
  }

  return QUIT_SUCCESS;
}
