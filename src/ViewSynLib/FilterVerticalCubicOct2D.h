#pragma once

#include "Filter.h"

template <class PixelType>
class FilterVerticalCubicOct2D : public Filter<PixelType>
{
public:
	FilterVerticalCubicOct2D()
	{
	}

	~FilterVerticalCubicOct2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterVerticalCubicOct2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, ipel[4], opel[8];
	int max_height = height + padding_size;
	int height_minus1 = height - 1;
	int w;

	for (i = -padding_size; i<max_height; i++)
	{
		ipel[0] = ImageTools::CLIP3(i - 1, 0, height_minus1);
		ipel[1] = ImageTools::CLIP3(i, 0, height_minus1);
		ipel[2] = ImageTools::CLIP3(i + 1, 0, height_minus1);
		ipel[3] = ImageTools::CLIP3(i + 2, 0, height_minus1);

		opel[0] = (i + padding_size) << 3;
		opel[1] = opel[0] + 1;
		opel[2] = opel[0] + 2;
		opel[3] = opel[0] + 3;
		opel[4] = opel[0] + 4;
		opel[5] = opel[0] + 5;
		opel[6] = opel[0] + 6;
		opel[7] = opel[0] + 7;

		for (w = 0; w<width; w++)
		{
			out[opel[0]][w] = in[ipel[1]][w];
			out[opel[1]][w] = ImageTools::CLIP3((62 * in[ipel[1]][w] + 9 * in[ipel[2]][w] - 6 * in[ipel[0]][w] - in[ipel[3]][w] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[opel[2]][w] = ImageTools::CLIP3((57 * in[ipel[1]][w] + 19 * in[ipel[2]][w] - 9 * in[ipel[0]][w] - 3 * in[ipel[3]][w] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[opel[3]][w] = ImageTools::CLIP3((49 * in[ipel[1]][w] + 30 * in[ipel[2]][w] - 9 * in[ipel[0]][w] - 6 * in[ipel[3]][w] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[opel[4]][w] = ImageTools::CLIP3((40 * in[ipel[1]][w] + 40 * in[ipel[2]][w] - 8 * in[ipel[0]][w] - 8 * in[ipel[3]][w] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[opel[5]][w] = ImageTools::CLIP3((30 * in[ipel[1]][w] + 49 * in[ipel[2]][w] - 6 * in[ipel[0]][w] - 9 * in[ipel[3]][w] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[opel[6]][w] = ImageTools::CLIP3((19 * in[ipel[1]][w] + 57 * in[ipel[2]][w] - 3 * in[ipel[0]][w] - 9 * in[ipel[3]][w] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[opel[7]][w] = ImageTools::CLIP3((9 * in[ipel[1]][w] + 62 * in[ipel[2]][w] - in[ipel[0]][w] - 6 * in[ipel[3]][w] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}
