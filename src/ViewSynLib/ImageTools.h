#pragma once

#include "SystemIncludes.h"

class ImageTools
{
public:
	
	static double CLIP3(double x, int min, int max)
	{
		return (x) < (min) ? (min) : ((x) > (max) ? (max) : (x));
	}

	static int CLIP3(int x, int min, int max)
	{
		return (x) < (min) ? (min) : ((x) > (max) ? (max) : (x));
	}

	static double TURN3(double x, int min, int max)
	{
		return (x) < (min) ? (-x) : ((x) > (max) ? (2 * (max)-(x)) : (x));
	}

	static int TURN3(int x, int min, int max)
	{
		return (x) < (min) ? (-x) : ((x) > (max) ? (2 * (max)-(x)) : (x));
	}
	
	
};