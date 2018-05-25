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
	bool apply(View* view);

private:
	IplConvKernel * m_convKernel;

	Image<HoleType>* m_imgBound;

};
