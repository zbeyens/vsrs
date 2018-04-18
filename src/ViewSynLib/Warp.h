#pragma once

#include "View.h"

class Warp
{
public:
	Warp(View* view)
		: m_view(view)
	{
	}

	~Warp()
	{
	}

	virtual bool init();

	virtual bool apply(ImageType ***src) { return true; }
	virtual bool apply(DepthType **pDepthMap) { return true; }

protected:
	/*
		Erode a bound image (pixel value 255 if bound, 0 else) : for each pixel=255, we apply erosion: 
		set all the next pixel=255 to 0 till the end of the line
		If flag=1, set the last pixel processed to 255.
		If flag=0, set the first pixel processed to 255.
	*/
	void erodebound(IplImage* bound, int flag);

	View* m_view;

	ConfigSyn& cfg = ConfigSyn::getInstance();

};
