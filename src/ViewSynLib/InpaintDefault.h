#pragma once

#include "Inpaint.h"

class InpaintDefault : public Inpaint
{
public:
	InpaintDefault()
	{
	}

	~InpaintDefault()
	{
	}

	void apply(Image<ImageType>* outputImage, Image<ImageType>* inputImage, Image<DepthType>* inputDepth, Image<ImageType>* holesMask, vector<View*> views);
};
