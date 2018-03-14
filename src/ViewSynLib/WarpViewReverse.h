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

	bool apply(ImageType ***src);

private:
	IplConvKernel * m_convKernel;

};
