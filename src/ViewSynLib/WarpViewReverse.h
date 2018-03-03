#pragma once

#include "Warp.h"

class WarpViewReverse : public Warp
{
public:
	WarpViewReverse(View* view)
		: Warp(view)
	{
	}

	~WarpViewReverse();

	bool init();

	bool apply(ImageType ***src, DepthType **pDepthMap, int th_same_depth = 5);

private:
	IplConvKernel * m_convKernel;

};
