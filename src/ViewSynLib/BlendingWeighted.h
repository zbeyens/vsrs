#pragma once

#include "BlendingHoles.h"

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
	void apply(vector<View*> views, Image<ImageType>* holesMask);

private:
	ConfigSyn & cfg = ConfigSyn::getInstance();

};

