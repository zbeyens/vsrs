#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "View.h"

class Inpaint
{
public:
	Inpaint()
	{
	}

	~Inpaint()
	{
	}

	virtual void apply(Image<ImageType>* synImage, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask, vector<View*> views) = 0;

protected:
	ConfigSyn & cfg = ConfigSyn::getInstance();
};
