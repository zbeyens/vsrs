#pragma once

#include "Filter.h"

template <class PixelType>
class FilterVerticalLinearQuarter2D : public Filter<PixelType>
{
public:
	FilterVerticalLinearQuarter2D()
	{
	}

	~FilterVerticalLinearQuarter2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);
};

template<class PixelType>
inline void FilterVerticalLinearQuarter2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, top, bottom;
	int max_height = height + padding_size;
	int height_minus1 = height - 1;
	int w, pel[4];

	for (i = -padding_size; i < max_height; i++)
	{
		top = ImageTools::CLIP3(i, 0, height_minus1);
		bottom = ImageTools::CLIP3(i + 1, 0, height_minus1);
		pel[0] = (i + padding_size) << 2;
		pel[1] = pel[0] + 1;
		pel[2] = pel[1] + 1;
		pel[3] = pel[2] + 1;
		for (w = 0; w < width; w++)
		{
			out[pel[0]][w] = in[top][w];
			out[pel[1]][w] = (in[top][w] * 3 + in[bottom][w] + 2) >> 2;
			out[pel[2]][w] = (in[top][w] + in[bottom][w] + 1) >> 1;
			out[pel[3]][w] = (in[top][w] + in[bottom][w] * 3 + 2) >> 2;
		}
	}
}
