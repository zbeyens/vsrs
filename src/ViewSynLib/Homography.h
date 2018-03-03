#pragma once

#include "ConfigSyn.h"

class Homography
{
public:
	Homography()
	{
	}

	~Homography()
	{
	}

	bool apply(CvMat *&matH_F2TOut, CvMat *&matH_T2FOut, const CvMat *MatInFrom, const CvMat *MatExFrom, const CvMat *MatProjTo);
};
