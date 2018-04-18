#pragma once

#include "BlendingHoles.h"

class BlendingAll : public BlendingHoles
{
public:
	BlendingAll()
	{
	}

	~BlendingAll()
	{
	}

	// blend holes - fill L / R holes fillable by R / L
	void apply(vector<View*> views);

private:
	ConfigSyn & cfg = ConfigSyn::getInstance();

};

