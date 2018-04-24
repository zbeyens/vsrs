#pragma once

#include "SystemIncludes.h"

class Tools
{
public:
	//! Free T if not null
	template<typename T>
	static void safeFree(T p)
	{
		if (p != NULL)
		{
			free(p);
			p = NULL;
		}
	}
	
	//! Delete T if not null
	template<typename T>
	static void safeDelete(T p)
	{
		if (p != NULL)
		{
			delete p;
			p = NULL;
		}
	}

	//! Delete T if not null
	template<typename T>
	static void safeDeleteP(T p)
	{
		if (p != NULL)
		{
			delete[] p;
			p = NULL;
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

	
	//static double CLIP3(double x, int min, int max)
	//{
	//	return (x) < (min) ? (min) : ((x) > (max) ? (max) : (x));
	//}

	//static int CLIP3(int x, int min, int max)
	//{
	//	return (x) < (min) ? (min) : ((x) > (max) ? (max) : (x));
	//}

	
};