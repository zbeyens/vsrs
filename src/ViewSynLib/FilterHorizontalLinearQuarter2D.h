#pragma once

#include "Filter.h"

template <class PixelType>
class FilterHorizontalLinearQuarter2D : public Filter<PixelType>
{
public:
	FilterHorizontalLinearQuarter2D()
	{
	}

	~FilterHorizontalLinearQuarter2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterHorizontalLinearQuarter2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, left, right;
	int max_width = width + padding_size;
	int width_minus1 = width - 1;
	int h, pel[4];

	for (i = -padding_size; i < max_width; i++)
	{
		left = ImageTools::CLIP3(i, 0, width_minus1);
		right = ImageTools::CLIP3(i + 1, 0, width_minus1);
		pel[0] = (i + padding_size) << 2;
		pel[1] = pel[0] + 1;
		pel[2] = pel[1] + 1;
		pel[3] = pel[2] + 1;
		for (h = 0; h < height; h++)
		{
			out[h][pel[0]] = in[h][left];
			out[h][pel[1]] = (in[h][left] * 3 + in[h][right] + 2) >> 2;
			out[h][pel[2]] = (in[h][left] + in[h][right] + 1) >> 1;
			out[h][pel[3]] = (in[h][left] + in[h][right] * 3 + 2) >> 2;
		}
	}
}
