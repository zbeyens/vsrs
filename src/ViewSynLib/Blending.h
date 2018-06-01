#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "View.h"

/*!
	Blending algorithm
*/
class Blending
{
public:
	Blending()
	{
	}

	~Blending()
	{
	}

	/*!
		For each pixel position where there are no holes on the synthesized images, if the
		depth difference is less than ”DepthBlendDiff”, a weighted blending is applied.
		Otherwise, the pixel of the view having the bigger depth value is chosen.
	*/
	virtual void apply(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> blendedImage, shared_ptr<Image<DepthType>> blendedDepth, shared_ptr<Image<ImageType>> holesMask) = 0;

protected:
	ConfigSyn & cfg = ConfigSyn::getInstance();

};

