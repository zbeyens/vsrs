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
	void apply(shared_ptr<Image<ImageType>> synImage, shared_ptr<Image<ImageType>> blendedImage, shared_ptr<Image<DepthType>> blendedDepth, shared_ptr<Image<ImageType>> holesMask, vector<shared_ptr<View>> views);

};

