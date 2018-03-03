#pragma once

#include "Warp.h"

class WarpIpelViewReverse : public Warp
{
public:
	WarpIpelViewReverse(View* view)
		: Warp(view)
	{
	}

	~WarpIpelViewReverse()
	{
	}

	bool apply(ImageType ***src, DepthType **pDepthMap, int th_same_depth = 5);

};
