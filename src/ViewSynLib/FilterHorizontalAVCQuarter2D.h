#pragma once

#include "Filter.h"

template <class PixelType>
class FilterHorizontalAVCQuarter2D: public Filter<PixelType>
{
public:
	FilterHorizontalAVCQuarter2D()
	{
	}

	~FilterHorizontalAVCQuarter2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterHorizontalAVCQuarter2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i;
	int max_width = width + padding_size;
	int width_minus1 = width - 1;
	int h, ipel[6], opel[4];

	for (i = -padding_size; i < max_width; i++)
	{
		ipel[0] = ImageTools::CLIP3(i - 2, 0, width_minus1);
		ipel[1] = ImageTools::CLIP3(i - 1, 0, width_minus1);
		ipel[2] = ImageTools::CLIP3(i, 0, width_minus1);
		ipel[3] = ImageTools::CLIP3(i + 1, 0, width_minus1);
		ipel[4] = ImageTools::CLIP3(i + 2, 0, width_minus1);
		ipel[5] = ImageTools::CLIP3(i + 3, 0, width_minus1);

		opel[0] = (i + padding_size) << 2;
		opel[1] = opel[0] + 1;
		opel[2] = opel[1] + 1;
		opel[3] = opel[2] + 1;
		for (h = 0; h < height; h++)
		{
			out[h][opel[0]] = in[h][ipel[2]];
			out[h][opel[2]] = ImageTools::CLIP3((20 * (in[h][ipel[2]] + in[h][ipel[3]]) - 5 * (in[h][ipel[1]] + in[h][ipel[4]]) + (in[h][ipel[0]] + in[h][ipel[5]]) + 16) >> 5, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[1]] = (in[h][ipel[2]] + out[h][opel[2]] + 1) >> 1;
			out[h][opel[3]] = (in[h][ipel[3]] + out[h][opel[2]] + 1) >> 1;
		}
	}
}
