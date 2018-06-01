#pragma once

#include "BlendingHoles.h"

/*!
	Blending the closer image holes algorithm
*/
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
	void apply(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> holesMask);

};

