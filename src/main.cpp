// std
#include <chrono>

// libs
#include "cxxopts.hpp"

// my bs
#include "utilities.h"
#include "mip_chain_fill.h"
#include "error_msg.h"

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    return QUIT_ERROR_INPUT;
  }

  // input options
  bool verbose = false;
  std::string resize = "";
  std::string inputpath = "";
  std::string outputpath = "";

  // read input arguments and populate above variables
  try
  {
    cxxopts::Options options("test", "A brief description");
    options.positional_help("<input file> <output file (optional)>");

    TODO("make help more readable")
    options.add_options()
      ("r,resize", "Resize non power of two -r larger, -r smaller, -r nearest", cxxopts::value<std::string>()->default_value("nearest"))
      ("v,verbose", "Print status", cxxopts::value<bool>()->default_value("false"))
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

    if (result.count("verbose"))
      verbose = result["verbose"].as<bool>();

    if (result.count("resize"))
      resize = result["resize"].as<std::string>();

    if (result.count("input"))
      inputpath = result["input"].as<std::string>();

    if (result.count("output"))
      outputpath = result["output"].as<std::string>();
  }
  catch (std::exception const& error)
  {
    std::cerr << ERR_MSG_ARGV << error.what() << std::endl;
    return QUIT_ERROR_INPUT;
  }

  // input arguments are valid
  // check input path
  if (!FileExists(inputpath) || !IsPNG(inputpath))
  {
    std::cout << ERR_MSG_INPUT << inputpath << std::endl;
    return QUIT_ERROR_INPUT;
  }

  // input path is valid
  // now the work may begin
  try
  {
    png::image< png::rgba_pixel > image(inputpath);

    png::uint_32 originalWidth =  image.get_width();
    png::uint_32 originalHeight = image.get_height();

    png::uint_32 pow2Width = originalWidth;
    png::uint_32 pow2Height = originalHeight;

    NonPowerOfTwoResize resizeOption = NonPowerOfTwoResize::NEAREST;

    std::chrono::steady_clock::time_point startTime;

    if (StringCompare(resize, "larger"))
    {
      resizeOption = NonPowerOfTwoResize::NEXT_LARGEST;
    }
    else if (StringCompare(resize, "smaller"))
    {
      resizeOption = NonPowerOfTwoResize::NEXT_SMALLEST;
    }

    bool doResize = false;

    if (!IsPowerOfTwo(originalWidth))
    {
      if (verbose)
      {
        std::cout << ERR_MSG_POWER2 << std::endl;
      }
      pow2Width = CalculatePowerOfTwo(originalWidth, resizeOption);
      doResize = true;
    }

    if (!IsPowerOfTwo(originalHeight))
    {
      if (verbose)
      {
        std::cout << ERR_MSG_POWER2 << std::endl;
      }
      pow2Height = CalculatePowerOfTwo(originalHeight, resizeOption);
      doResize = true;
    }

    if (doResize)
    {
      if (verbose)
      {
        std::cout << "Start resize to " << resize << std::endl;
        startTime = std::chrono::high_resolution_clock::now();
      }

      Resize(image, pow2Width, pow2Height);

      if (verbose)
      {
        auto time = std::chrono::high_resolution_clock::now() - startTime;
        std::cout << "Resize took " << time / std::chrono::milliseconds(1) << " ms.\n" << std::endl;
      }
    }

    MipChainFill fill = MipChainFill(image);

    if (verbose)
    {
      std::cout << "Start color mip map process" << std::endl;
      startTime = std::chrono::high_resolution_clock::now();
    }

    png::image< png::rgba_pixel > output = fill.CompositeAlphaMip();

    if (verbose)
    {
      auto time = std::chrono::high_resolution_clock::now() - startTime;
      std::cout << "Color mip map process took " << time / std::chrono::milliseconds(1) << " ms." << std::endl;
    }

    output.write(outputpath);
  }
  catch (std::exception const& error)
  {
    std::cerr << ERR_MSG_OUTPUT << error.what() << std::endl;
    return QUIT_ERROR_OTHER;
  }

  return QUIT_SUCCESS;
}
