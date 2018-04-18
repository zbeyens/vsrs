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

	virtual void apply(vector<View*> views) = 0;

protected:
	ConfigSyn & cfg = ConfigSyn::getInstance();
};
