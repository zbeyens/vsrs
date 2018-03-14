#pragma once

#include "Filter.h"

template <class PixelType>
class FilterHorizontalAVCHalf2D: public Filter<PixelType>
{
public:
	FilterHorizontalAVCHalf2D()
	{
	}

	~FilterHorizontalAVCHalf2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterHorizontalAVCHalf2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, pel[6];
	int max_width = width + padding_size;
	int width_minus1 = width - 1;
	int h, int_pel, half_pel;

	for (i = -padding_size; i < max_width; i++)
	{
		pel[0] = ImageTools::CLIP3(i - 2, 0, width_minus1);
		pel[1] = ImageTools::CLIP3(i - 1, 0, width_minus1);
		pel[2] = ImageTools::CLIP3(i, 0, width_minus1);
		pel[3] = ImageTools::CLIP3(i + 1, 0, width_minus1);
		pel[4] = ImageTools::CLIP3(i + 2, 0, width_minus1);
		pel[5] = ImageTools::CLIP3(i + 3, 0, width_minus1);

		int_pel = (i + padding_size) << 1;
		half_pel = int_pel + 1;
		for (h = 0; h < height; h++)
		{
			out[h][int_pel] = in[h][pel[2]];
			out[h][half_pel] = ImageTools::CLIP3((20 * (in[h][pel[2]] + in[h][pel[3]]) - 5 * (in[h][pel[1]] + in[h][pel[4]]) + (in[h][pel[0]] + in[h][pel[5]]) + 16) >> 5, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}
