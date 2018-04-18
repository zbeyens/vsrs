#ifndef _INCLUDE_VIEW_INTERPOLATION_H_
#define _INCLUDE_VIEW_INTERPOLATION_H_

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "ViewSynthesis3D.h"
#include "ViewSynthesis1D.h"
#include "BoundaryNoiseRemoval.h"
#include "Image.h"
#include "AlgoFactory.h"
#include "Filter.h"

/*
 * Interface class between application and synthesis objects
 */
class Application
{
public:
	// init fields to NULL
	Application();
	virtual ~Application();

	bool init();
	bool upsampleImage(int index, unique_ptr<Image<ImageType>>& pcYuv);	//!> convert the YUV444 image into BGR or YUV and then upsample *
	bool DoViewInterpolation(unique_ptr<Image<ImageType>>& pYuvBuffer);	//!> The main interface function to be called to perform view interpolation

	Image<DepthType> getDepthMapLeft() { return m_depthLeft; }
	Image<DepthType> getDepthMapRight() { return m_depthRight; }

	int getFrameNumber() { return m_iFrameNumber; }	//Zhejiang

	void setFrameNumber(int frame_number) { m_iFrameNumber = frame_number; }

private:
	bool xViewSynthesis3D(unique_ptr<Image<ImageType>>& pYuvBuffer);
	bool xViewSynthesis1D(unique_ptr<Image<ImageType>>& pSynYuvBuffer);
	bool xBoundaryNoiseRemoval(Image<ImageType>* pRefLeft, Image<ImageType>* pRefRight, Image<DepthType>* pRefDepthLeft, Image<DepthType>* pRefDepthRight, Image<HoleType>* pRefHoleLeft, Image<HoleType>* pRefHoleRight, unique_ptr<Image<ImageType>>& pSynYuvBuffer, bool SynthesisMode);

private:
	void initInputImages();
	void initOutputImages();

	ConfigSyn& cfg;

	int m_iFrameNumber;		     //used in TIM, Zhejiang

	int m_height;	//!> Source height
	int m_width;	//!> Source width
	int m_width2;	//!> The width when the ref view is upsampled (may be 2*Width or 4*Width, it depends). otherwise, it is the same as Width

	vector<Image<ImageType>> m_imagesNotUpsampled;	//!> input read (w, h) not upsampled, stored in BGR or YUV format
	vector<Image<ImageType>> m_images;				//!> To store the image of the right reference view: (w*precision, h)

	Image<DepthType> m_depthLeft;		//!> To store the depth map of the left reference view
	Image<DepthType> m_depthRight;		//!> To store the depth map of the right reference view

	ViewSynthesis3D m_viewSynthesis3D;			//!> The object to do 3D warping view synthesis
	ViewSynthesis1D m_viewSynthesis1D;			//!> The object to do 1D view synthesis
	BoundaryNoiseRemoval m_boundaryNoiseRemoval;	//!> The object to do boundary noise removal

	// result
	unsigned char* m_synColorLeft;
	unsigned char* m_synColorRight;
	unsigned char* m_synDepthLeft;
	unsigned char* m_synDepthRight;
};

#endif