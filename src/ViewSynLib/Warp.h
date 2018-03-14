#pragma once

#include "View.h"

class Warp
{
public:
	Warp(View* view)
		: mView(view)
	{
	}

	~Warp()
	{
	}

	virtual bool init();

	virtual bool apply(ImageType ***src) { return true; }
	virtual bool apply(DepthType **pDepthMap) { return true; }

protected:
	void erodebound(IplImage* bound, int flag);

	View* mView;

	ConfigSyn& cfg = ConfigSyn::getInstance();

};
