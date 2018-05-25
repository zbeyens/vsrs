#pragma once

#include "Filter.h"

template <class PixelType>
class FilterHorizontalCubicOct2D : public Filter<PixelType>
{
public:
	FilterHorizontalCubicOct2D()
	{
	}

	~FilterHorizontalCubicOct2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);

};


template<class PixelType>
inline void FilterHorizontalCubicOct2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i, ipel[4], opel[8];
	int max_width = width + padding_size;
	int width_minus1 = width - 1;
	int h;

	for (i = -padding_size; i<max_width; i++)
	{
		ipel[0] = ImageTools::CLIP3(i - 1, 0, width_minus1);//NICT
		ipel[1] = ImageTools::CLIP3(i, 0, width_minus1);//NICT
		ipel[2] = ImageTools::CLIP3(i + 1, 0, width_minus1);//NICT
		ipel[3] = ImageTools::CLIP3(i + 2, 0, width_minus1);//NICT

		opel[0] = (i + padding_size) << 3;
		opel[1] = opel[0] + 1;
		opel[2] = opel[0] + 2;
		opel[3] = opel[0] + 3;
		opel[4] = opel[0] + 4;
		opel[5] = opel[0] + 5;
		opel[6] = opel[0] + 6;
		opel[7] = opel[0] + 7;

		for (h = 0; h<height; h++)
		{
			out[h][opel[0]] = in[h][ipel[1]];
			out[h][opel[1]] = ImageTools::CLIP3((62 * in[h][ipel[1]] + 9 * in[h][ipel[2]] - 6 * in[h][ipel[0]] - in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[2]] = ImageTools::CLIP3((57 * in[h][ipel[1]] + 19 * in[h][ipel[2]] - 9 * in[h][ipel[0]] - 3 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[3]] = ImageTools::CLIP3((49 * in[h][ipel[1]] + 30 * in[h][ipel[2]] - 9 * in[h][ipel[0]] - 6 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[4]] = ImageTools::CLIP3((40 * in[h][ipel[1]] + 40 * in[h][ipel[2]] - 8 * in[h][ipel[0]] - 8 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[5]] = ImageTools::CLIP3((30 * in[h][ipel[1]] + 49 * in[h][ipel[2]] - 6 * in[h][ipel[0]] - 9 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[6]] = ImageTools::CLIP3((19 * in[h][ipel[1]] + 57 * in[h][ipel[2]] - 3 * in[h][ipel[0]] - 9 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
			out[h][opel[7]] = ImageTools::CLIP3((9 * in[h][ipel[1]] + 62 * in[h][ipel[2]] - in[h][ipel[0]] - 6 * in[h][ipel[3]] + 32) >> 6, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}
