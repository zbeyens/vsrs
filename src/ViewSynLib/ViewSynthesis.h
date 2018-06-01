#pragma once

#include "ConfigSyn.h"
#include "Image.h"
#include "AlgoFactory.h"
#include "View.h"

/*!
	Base pipeline
*/
class ViewSynthesis
{
public:
	ViewSynthesis();

	~ViewSynthesis();

	/*!
		Init all the Views and init their input image and depth map
	*/
	virtual bool init();
	virtual bool apply(shared_ptr<Image<ImageType>> outImg) = 0;

	/*!
		Set the src image to views[index] image not yet upsampled and convert it to YUV444 or RGB
	*/
	void setInputImage(int index, shared_ptr<Image<ImageType>> src);	//!> convert the YUV444 image into BGR or YUV and then upsample *
	void setFrameNumber(int frame_number) { m_frameNumber = frame_number; }

	shared_ptr<View> getView(uint index) { return m_views[index]; }
	shared_ptr<Image<DepthType>> getSynDepth() { return m_synDepth; }
	int getFrameNumber() { return m_frameNumber; }	//Zhejiang

protected:
	/*!
		Upsample the imageNotUpsampled to image of each view.
	*/
	void upsampleViews();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	int m_frameNumber;		     //!> frame number - used in TIM, Zhejiang

	vector<shared_ptr<View>>  m_views; //!> all the reference views (minimum 2)
	shared_ptr<Image<DepthType>> m_synDepth;     //!> depth map after blending
};
