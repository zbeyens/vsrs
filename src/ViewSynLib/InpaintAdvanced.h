#pragma once

#include "Inpaint.h"

class InpaintAdvanced : public Inpaint
{
public:
	InpaintAdvanced()
	{
	}

	~InpaintAdvanced()
	{
	}

	void apply(Image<ImageType>* synImage, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask, vector<View*> views);

};

