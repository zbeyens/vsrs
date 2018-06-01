#pragma once

#include "ConfigSyn.h"

/*!
	Filter algorithm for upsampling
*/
template <class PixelType>
class Filter
{
public:
	Filter()
	{
	}

	~Filter()
	{
	}

	virtual void apply(PixelType **in, PixelType **out, int width, int height, int padding_size) {}
};
