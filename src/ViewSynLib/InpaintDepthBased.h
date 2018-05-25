#pragma once

#include "Inpaint.h"

class InpaintDepthBased : public Inpaint
{
public:
	InpaintDepthBased()
	{
	}

	~InpaintDepthBased()
	{
	}

	// TO UPDATE BY NICT
	void apply(Image<ImageType>* outputImage, Image<ImageType>* inputImage, Image<DepthType>* inputDepth, Image<ImageType>* holesMask, vector<View*> views);

};

