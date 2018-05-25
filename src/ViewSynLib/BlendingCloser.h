#pragma once

#include "BlendingHoles.h"

class BlendingCloser : public BlendingHoles
{
public:
	BlendingCloser()
	{
	}

	~BlendingCloser()
	{
	}

	// blend images - choose and fill the closer image holes + put its fills on the other image
	void apply(vector<View*> views, Image<ImageType>* holesMask);

};

