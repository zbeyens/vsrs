#pragma once

#include "Blending.h"

class Blending2V : public Blending
{
public:
	Blending2V()
	{
	}

	~Blending2V()
	{
	}

	void apply(vector<View*> views, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask);

private:
	void blendPixel(Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* ImageLeft, Image<DepthType>* DepthLeft, int ptv);

};

