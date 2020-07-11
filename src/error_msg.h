#pragma once

#define QUIT_SUCCESS 0
#define QUIT_ERROR_INPUT 1
#define QUIT_ERROR_OTHER 2

#define ERR_MSG_POWER2 "WARNING: Dimension of image is not power of 2. Attempting resize.\nShould the output not be as expected please manually resize to power of 2.\n"
#define ERR_MSG_ARGV "ERROR: Exception while evaluating arguments. "
#define ERR_MSG_INPUT "ERROR: Input file is not available. "
#define ERR_MSG_OUTPUT "ERROR: Exception while generating image: "