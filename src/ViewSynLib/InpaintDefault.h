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

	void apply(Image<ImageType>* synImage, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask, vector<View*> views);
};
