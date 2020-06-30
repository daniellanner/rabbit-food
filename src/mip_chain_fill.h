#pragma once
#include "utilities.h"

class MipChainFill
{
public:
	MipChainFill(alpha_img img);
	alpha_img CompositeAlphaMip();

private:
	alpha_img AlphaMip(const alpha_img& input);

	alpha_img _originalImage;
	png::uint_32 _width;
	png::uint_32 _height;
};