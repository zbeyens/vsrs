#pragma once

#include "Warp.h"

class WarpIpelDepth : public Warp
{
public:
	WarpIpelDepth(View* view)
		: Warp(view)
	{
	}

	~WarpIpelDepth()
	{
	}

	bool apply(ImageType ***src, DepthType **pDepthMap, int th_same_depth = 5);

private:
	void    cvexMedian(IplImage* dst);
	void    cvexBilateral(IplImage* dst, int sigma_d, int sigma_c);

};
