#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "View.h"

class Blending
{
public:
	Blending()
	{
	}

	~Blending()
	{
	}

	// blend holes - fill L / R holes fillable by R / L
	void apply(vector<View*> views, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask, double totalBaseline);

private:
	bool isPixelNearer(DepthType left, DepthType right);
	void blendNearerPixel(Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* ImageLeft, Image<DepthType>* DepthLeft, int ptv);

	ConfigSyn & cfg = ConfigSyn::getInstance();

};

