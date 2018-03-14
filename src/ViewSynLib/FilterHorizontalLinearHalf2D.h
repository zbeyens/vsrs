#pragma once

#include "Filter.h"

template <class PixelType>
class FilterHorizontalLinearHalf2D : public Filter<PixelType>
{
public:
	FilterHorizontalLinearHalf2D()
	{
	}

	~FilterHorizontalLinearHalf2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterHorizontalLinearHalf2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, left, right;
	int max_width = width + padding_size;
	int width_minus1 = width - 1;
	int h, int_pel, half_pel;

	for (i = -padding_size; i < max_width; i++)
	{
		left = ImageTools::CLIP3(i, 0, width_minus1);
		right = ImageTools::CLIP3(i + 1, 0, width_minus1);
		int_pel = (i + padding_size) << 1;
		half_pel = int_pel + 1;
		for (h = 0; h < height; h++)
		{
			out[h][int_pel] = in[h][left];
			out[h][half_pel] = (in[h][left] + in[h][right] + 1) >> 1;
		}
	}
}
