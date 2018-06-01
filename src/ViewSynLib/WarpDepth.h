#pragma once

#include "Warp.h"

class WarpDepth : public Warp
{
public:
	WarpDepth()
	{
	}

	~WarpDepth()
	{
	}
	
	bool init();

	/*!
		TO UPDATE BY NICT
	*/
	bool apply(shared_ptr<View> view);

private:
	Image<ImageType> m_imgTemp[2]; //express holes after smoothing
	Image<DepthType> m_imgDepthTemp[2];

};
