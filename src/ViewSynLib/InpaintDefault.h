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

	void apply(shared_ptr<Image<ImageType>> outImage, shared_ptr<Image<ImageType>> inImage, shared_ptr<Image<DepthType>> inDepth, shared_ptr<Image<ImageType>> holesMask, vector<shared_ptr<View>> views);
};
