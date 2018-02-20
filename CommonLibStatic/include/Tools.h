#pragma once

#include "SystemIncludes.h"

class Tools
{
public:
	//! Free T if not null
	template<typename T>
	static void safeFree(T p)
	{
		if ((p) != NULL)
		{
			free(p);
			(p) = NULL;
		}
	}
	
	//! Delete T if not null
	template<typename T>
	static void safeDelete(T p)
	{
		if ((p) != NULL)
		{
			delete p;
		}
	}

	//! Release IplImage
	static void safeReleaseImage(IplImage* image)
	{
		if ((image) != NULL) 
		{
			cvReleaseImage(&(image));
			(image) = NULL;
		}
	}

	//! Release CvMat
	static void safeReleaseMat(CvMat* mat)
	{
		if ((mat) != NULL)
		{
			cvReleaseMat(&(mat));
			(mat) = NULL;
		}
	}
};