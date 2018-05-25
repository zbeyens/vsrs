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

	// TO UPDATE BY NICT
	virtual void apply(vector<View*> views, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask) = 0;

protected:
	ConfigSyn & cfg = ConfigSyn::getInstance();

};

