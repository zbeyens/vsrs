#pragma once

#include "Warp.h"

class WarpIpelViewReverse : public Warp
{
public:
	WarpIpelViewReverse();

	~WarpIpelViewReverse()
	{
	}

	bool apply(View* view);

	Image<HoleType>* m_imgBound;

};
