#pragma once

#include "ConfigSyn.h"
#include "Image.h"
#include "AlgoFactory.h"
#include "BoundaryNoiseRemoval.h"
#include "View.h"


class ViewSynthesis
{
public:
	ViewSynthesis();

	~ViewSynthesis();

	virtual bool init();
	virtual bool apply(unique_ptr<Image<ImageType>>& pYuvBuffer) { return true; }
	virtual bool apply(Image<ImageType>& RefLeft, Image<ImageType>& RefRight, Image<DepthType>& RefDepthLeft, Image<DepthType>& RefDepthRight, unique_ptr<Image<ImageType>>& pSynYuvBuffer) { return true; }
	void setImage(int index, unique_ptr<Image<ImageType>>& pcYuv);	//!> convert the YUV444 image into BGR or YUV and then upsample *
	
	
	int getFrameNumber() { return m_iFrameNumber; }	//Zhejiang
	View* getView(uint index) { return m_views[index]; }


	void setFrameNumber(int frame_number) { m_iFrameNumber = frame_number; }


protected:
	void initViews();
	void initOutputImages();
	void upsample();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	int m_iFrameNumber;		     //used in TIM, Zhejiang

	int m_height;		//!> Source height
	int m_width;		//!> Source width
	int m_width2;		//!> The width when the ref view is upsampled (may be 2*Width or 4*Width, it depends). otherwise, it is the same as Width
	int m_imagesWidth;	//!> Width of the images upsampled. If the width is the same than the source image, there is no upsampling.

	vector<View*>  m_views;

	// result
	unsigned char* m_synColorLeft;
	unsigned char* m_synColorRight;
	unsigned char* m_synDepthLeft;
	unsigned char* m_synDepthRight;

};
