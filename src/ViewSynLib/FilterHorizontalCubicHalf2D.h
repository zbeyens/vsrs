#pragma once

#include "Filter.h"

template <class PixelType>
class FilterHorizontalCubicHalf2D : public Filter<PixelType>
{
public:
	FilterHorizontalCubicHalf2D()
	{
	}

	~FilterHorizontalCubicHalf2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);
};

template<class PixelType>
inline void FilterHorizontalCubicHalf2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, ipel[4], opel[2];
	int max_width = width + padding_size;
	int width_minus1 = width - 1;
	int h;

	for (i = -padding_size; i < max_width; i++)
	{
		ipel[0] = ImageTools::CLIP3(i - 1, 0, width_minus1);//NICT
		ipel[1] = ImageTools::CLIP3(i, 0, width_minus1);//NICT
		ipel[2] = ImageTools::CLIP3(i + 1, 0, width_minus1);//NICT
		ipel[3] = ImageTools::CLIP3(i + 2, 0, width_minus1);//NICT

		opel[0] = (i + padding_size) << 1;
		opel[1] = opel[0] + 1;

		for (h = 0; h < height; h++)
		{
			out[h][opel[0]] = in[h][ipel[1]];
			out[h][opel[1]] = ImageTools::CLIP3((5 * (in[h][ipel[1]] + in[h][ipel[2]]) - in[h][ipel[0]] - in[h][ipel[3]] + 4) >> 3, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}
