#pragma once

#include "Warp.h"

class WarpViewReverse : public Warp
{
public:
	WarpViewReverse()
	{
	}

	~WarpViewReverse();

	bool init();

	// TO UPDATE BY NICT
	bool apply(shared_ptr<View> view);

private:
	IplConvKernel * m_convKernel;

	unique_ptr<Image<HoleType>> m_imgBound;

};
