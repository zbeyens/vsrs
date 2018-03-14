#pragma once

#include "Filter.h"

template <class PixelType>
class FilterDummy2D : public Filter<PixelType>
{
public:
	FilterDummy2D()
	{
	}

	~FilterDummy2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterDummy2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i;
	int h;
	int max_width = width + padding_size;
	int ipel;

	for (h = 0; h < height; h++)
		for (i = -padding_size; i < max_width; i++)
		{
			ipel = ImageTools::CLIP3(i, 0, width);
			out[h][i + padding_size] = in[h][ipel];
		}
}
