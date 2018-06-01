#pragma once

#include "View.h"

/*!
	Warping algorithm
*/
class Warp
{
public:
	Warp()
	{
	}

	~Warp()
	{
	}

	virtual bool apply(shared_ptr<View> view) = 0;

protected:
	/*
		Erode a bound image (pixel value 255 if bound, 0 else) : for each successive pixel=255, we apply erosion: 
		set all the next pixel=255 to 0 till the end of the line
		If flag=1, set the last pixel processed to 255.
		If flag=0, set the first pixel processed to 255.
	*/
	void erodebound(IplImage* bound, int flag);

	ConfigSyn& cfg = ConfigSyn::getInstance();

};
