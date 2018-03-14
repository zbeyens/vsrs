#pragma once

#include "Warp.h"

class WarpDepth : public Warp
{
public:
	WarpDepth(View* view)
		: Warp(view)
	{
	}

	~WarpDepth()
	{
	}
	
	bool init();

	bool apply(DepthType **pDepthMap);

private:
	Image<ImageType> m_imgTemp[2];
	Image<DepthType> m_imgDepthTemp[2];

};
