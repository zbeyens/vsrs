#pragma once

#include "BlendingHoles.h"

/*!
	Blending holes for left and right
*/
class BlendingWeighted : public BlendingHoles
{
public:
	BlendingWeighted()
	{
	}

	~BlendingWeighted()
	{
	}

	// blend holes - fill L / R holes fillable by R / L
	void apply(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> holesMask);

private:
	ConfigSyn & cfg = ConfigSyn::getInstance();

};

