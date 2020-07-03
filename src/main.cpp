#include "utilities.h"
#include "mip_chain_fill.h"
#include "cxxopts.hpp"

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    // no input file selected
    return 0;
  }

  TODO("file name as argv[1] and -f")
  std::string originPath = argv[0];
  std::string inputFile = argv[1];

  if (!FileExists(inputFile))
  {
    std::cout << "ERROR: Input file does not exist at location " << inputFile << std::endl;
    //return 1;
  }

  cxxopts::Options options("test", "A brief description");
  options.positional_help("<input file> <output file (optional)>");

  options.add_options()
    ("b,bar", "Param bar", cxxopts::value<std::string>())
    ("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))
    ("f,foo", "Param foo", cxxopts::value<int>()->default_value("10"))
    ("h,help", "Print usage")
    ("i,input", "Input file", cxxopts::value<std::string>())
    ("o,output", "Output file", cxxopts::value<std::string>())
    ("positional", "Positional parameters",
      cxxopts::value<std::string>())
    ;
    
  options.parse_positional({ "input", "positional" });
  auto result = options.parse(argc, argv);

  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    //exit(0);
  }
  bool debug = result["debug"].as<bool>();
  std::string bar;
  if (result.count("bar"))
    bar = result["bar"].as<std::string>();
  int foo = result["foo"].as<int>();


  std::cout << "Foo: " << foo << std::endl;
  std::cout << "Bar: " << bar << std::endl;


  if (result.count("bar"))
    bar = result["bar"].as<std::string>();

  std::cout << "Bar: " << bar << std::endl;

  if (result.count("input"))
    bar = result["input"].as<std::string>();

  std::cout << "First: " << bar << std::endl;

  //cxxopts::Options options("My Test", "a test");
  //options.add_options()
  //  ("f,file", "File name", cxxopts::value<std::string>())
  //  ("npt,nonpoweroftwo", "Non Power of Two", cxxopts::value<std::string>())
  //  ("t,threads", "Thread Count", cxxopts::value<int>())
  //  ("help", "Print help");

  //options.parse_positional("file");
  //auto result = options.parse(argc, argv);

  //std::cout << "File: " << result["file"].as<std::string>() << std::endl;

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

    TODO("input name")
    output.write("resized.png");
  }
  catch (std::exception const& error)
  {
    std::cerr << "pixel_generator: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
}
