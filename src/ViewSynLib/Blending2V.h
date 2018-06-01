#pragma once

#include "Blending.h"

/*!
	Blending two views algorithm
*/
class Blending2V : public Blending
{
public:
	Blending2V()
	{
	}

	~Blending2V()
	{
	}

	void apply(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> blendedImage, shared_ptr<Image<DepthType>> blendedDepth, shared_ptr<Image<ImageType>> holesMask);

private:
	void blendPixel(shared_ptr<Image<ImageType>> outImage, shared_ptr<Image<DepthType>> outDepth, shared_ptr<Image<ImageType>> inImage, shared_ptr<Image<DepthType>> inDepth, int ptv);

};

