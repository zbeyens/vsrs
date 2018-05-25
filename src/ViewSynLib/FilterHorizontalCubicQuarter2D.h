#pragma once

#include "Filter.h"

template <class PixelType>
class FilterHorizontalCubicQuarter2D : public Filter<PixelType>
{
public:
	FilterHorizontalCubicQuarter2D()
	{
	}

	~FilterHorizontalCubicQuarter2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterHorizontalCubicQuarter2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, ipel[4], opel[4];
	int max_width = width + padding_size;
	int width_minus1 = width - 1;
	int h;

	for (i = -padding_size; i < max_width; i++)
	{
		ipel[0] = ImageTools::CLIP3(i - 1, 0, width_minus1);
		ipel[1] = ImageTools::CLIP3(i, 0, width_minus1);
		ipel[2] = ImageTools::CLIP3(i + 1, 0, width_minus1);
		ipel[3] = ImageTools::CLIP3(i + 2, 0, width_minus1);

		opel[0] = (i + padding_size) << 2;
		opel[1] = opel[0] + 1;
		opel[2] = opel[0] + 2;
		opel[3] = opel[0] + 3;

		for (h = 0; h < height; h++)
		{
			out[h][opel[0]] = in[h][ipel[1]];
			out[h][opel[1]] = (PixelType)ImageTools::CLIP3((57 * in[h][ipel[1]] + 19 * in[h][ipel[2]] - 9 * in[h][ipel[0]] - 3 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[2]] = (PixelType)ImageTools::CLIP3((5 * (in[h][ipel[1]] + in[h][ipel[2]]) - in[h][ipel[0]] - in[h][ipel[3]] + 4) >> 3, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[3]] = (PixelType)ImageTools::CLIP3((19 * in[h][ipel[1]] + 57 * in[h][ipel[2]] - 3 * in[h][ipel[0]] - 9 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}
