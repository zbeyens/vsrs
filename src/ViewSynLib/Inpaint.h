#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "View.h"

/*!
	Inpainting the remaining holes
*/
class Inpaint
{
public:
	Inpaint()
	{
	}

	~Inpaint()
	{
	}

	virtual void apply(shared_ptr<Image<ImageType>> synImage, shared_ptr<Image<ImageType>> blendedImage, shared_ptr<Image<DepthType>> blendedDepth, shared_ptr<Image<ImageType>> holesMask, vector<shared_ptr<View>> views) = 0;

protected:
	ConfigSyn & cfg = ConfigSyn::getInstance();
};
