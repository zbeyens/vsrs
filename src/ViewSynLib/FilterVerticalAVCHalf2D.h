#pragma once

#include "Filter.h"

template <class PixelType>
class FilterVerticalAVCHalf2D : public Filter<PixelType>
{
public:
	FilterVerticalAVCHalf2D()
	{
	}

	~FilterVerticalAVCHalf2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);
};

template<class PixelType>
inline void FilterVerticalAVCHalf2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, pel[6];
	int max_height = height + padding_size;
	int height_minus1 = height - 1;
	int w, int_pel, half_pel;

	for (i = -padding_size; i < max_height; i++)
	{
		pel[0] = ImageTools::CLIP3(i - 2, 0, height_minus1);
		pel[1] = ImageTools::CLIP3(i - 1, 0, height_minus1);
		pel[2] = ImageTools::CLIP3(i, 0, height_minus1);
		pel[3] = ImageTools::CLIP3(i + 1, 0, height_minus1);
		pel[4] = ImageTools::CLIP3(i + 2, 0, height_minus1);
		pel[5] = ImageTools::CLIP3(i + 3, 0, height_minus1);

		int_pel = (i + padding_size) << 1;
		half_pel = int_pel + 1;
		for (w = 0; w < width; w++)
		{
			out[int_pel][w] = in[pel[2]][w];
			out[half_pel][w] = ImageTools::CLIP3((20 * (in[pel[2]][w] + in[pel[3]][w]) - 5 * (in[pel[1]][w] + in[pel[4]][w]) + (in[pel[0]][w] + in[pel[5]][w]) + 16) >> 5, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}
