#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "View.h"

class BlendingHoles
{
public:
	BlendingHoles()
	{
	}

	~BlendingHoles()
	{
	}

	virtual void apply(vector<View*> views, Image<ImageType>* holesMask) = 0;
	
	/**
		Init the fillable and non-fillable masks. Pixels holes (or unstable pixels) which will be replaced by pixels synthesized from the other view
	*/
	virtual void initHolesMasks(vector<View*> views, Image<ImageType>* holesMask);

protected:

	ConfigSyn & cfg = ConfigSyn::getInstance();
};

