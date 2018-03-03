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

	virtual bool apply(ImageType ***src, DepthType **pDepthMap, int th_same_depth = 5) = 0;

protected:
	void erodebound(IplImage* bound, int flag);

	View* mView;

	ConfigSyn& cfg = ConfigSyn::getInstance();

};
