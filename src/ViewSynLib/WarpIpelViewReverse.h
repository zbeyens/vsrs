#pragma once

#include "Warp.h"

class WarpIpelViewReverse : public Warp
{
public:
	WarpIpelViewReverse();

	~WarpIpelViewReverse()
	{
	}

	bool apply(shared_ptr<View> view);

	unique_ptr<Image<HoleType>> m_imgBound;

};
