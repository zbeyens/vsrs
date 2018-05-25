#pragma once

#include "Warp.h"

class WarpIpelDepth : public Warp
{
public:
	WarpIpelDepth()
	{
	}

	~WarpIpelDepth()
	{
	}

	bool apply(View* view);

private:
	void    cvexMedian(IplImage* dst);
	void    cvexBilateral(IplImage* dst, int sigma_d, int sigma_c);

};
