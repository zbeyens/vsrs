#pragma once

#include "Filter.h"

template <class PixelType>
class FilterVerticalAVCQuarter2D : public Filter<PixelType>
{
public:
	FilterVerticalAVCQuarter2D()
	{
	}

	~FilterVerticalAVCQuarter2D()
	{
	}

	void apply(PixelType **in, PixelType **out, int width, int height, int padding_size);
};

template<class PixelType>
inline void FilterVerticalAVCQuarter2D<PixelType>::apply(PixelType ** in, PixelType ** out, int width, int height, int padding_size)
{
	int i;
	int max_height = height + padding_size;
	int height_minus1 = height - 1;
	int w, ipel[6], opel[4];

	for (i = -padding_size; i < max_height; i++)
	{
		ipel[0] = ImageTools::CLIP3(i - 2, 0, height_minus1);
		ipel[1] = ImageTools::CLIP3(i - 1, 0, height_minus1);
		ipel[2] = ImageTools::CLIP3(i, 0, height_minus1);
		ipel[3] = ImageTools::CLIP3(i + 1, 0, height_minus1);
		ipel[4] = ImageTools::CLIP3(i + 2, 0, height_minus1);
		ipel[5] = ImageTools::CLIP3(i + 3, 0, height_minus1);

		opel[0] = (i + padding_size) << 2;
		opel[1] = opel[0] + 1;
		opel[2] = opel[1] + 1;
		opel[3] = opel[2] + 1;
		for (w = 0; w < width; w++)
		{
			out[opel[0]][w] = in[ipel[2]][w];
			out[opel[2]][w] = ImageTools::CLIP3((20 * (in[ipel[2]][w] + in[ipel[3]][w]) - 5 * (in[ipel[1]][w] + in[ipel[4]][w]) + (in[ipel[0]][w] + in[ipel[5]][w]) + 16) >> 5, 0, MaxTypeValue<PixelType>() - 1);
			out[opel[1]][w] = (in[ipel[2]][w] + out[opel[2]][w] + 1) >> 1;
			out[opel[3]][w] = (in[ipel[3]][w] + out[opel[2]][w] + 1) >> 1;
		}
	}
}
