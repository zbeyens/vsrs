#pragma once

#include "Filter.h"

template <class PixelType>
class FilterVerticalLinearHalf2D : public Filter<PixelType>
{
public:
	FilterVerticalLinearHalf2D()
	{
	}

	~FilterVerticalLinearHalf2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterVerticalLinearHalf2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, top, bottom;
	int max_height = height + padding_size;
	int height_minus1 = height - 1;
	int w, int_pel, half_pel;

	for (i = -padding_size; i<max_height; i++)
	{
		top = ImageTools::CLIP3(i, 0, height_minus1);
		bottom = ImageTools::CLIP3(i + 1, 0, height_minus1);
		int_pel = (i + padding_size) << 1;
		half_pel = int_pel + 1;
		for (w = 0; w<width; w++)
		{
			out[int_pel][w] = in[top][w];
			out[half_pel][w] = (in[top][w] + in[bottom][w] + 1) >> 1;
		}
	}
}
