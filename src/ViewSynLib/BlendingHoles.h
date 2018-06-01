#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "View.h"

/*!
	Blending holes algorithm
*/
class BlendingHoles
{
public:
	BlendingHoles()
	{
	}

	~BlendingHoles()
	{
	}

	virtual void apply(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> holesMask) = 0;
	
	/*!
		Init the fillable and non-fillable masks. Pixels holes (or unstable pixels) which will be replaced by pixels synthesized from the other view
	*/
	virtual void initHolesMasks(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> holesMask);

protected:

	ConfigSyn & cfg = ConfigSyn::getInstance();
};

