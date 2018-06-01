#pragma once

#include "ConfigSyn.h"

/*!
	Homography algorithm
*/
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
