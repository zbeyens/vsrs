#ifndef _INCLUDE_VIEW_INTERPOLATION_H_
#define _INCLUDE_VIEW_INTERPOLATION_H_

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "ViewSynthesis3D.h"
#include "ViewSynthesis1D.h"
#include "BoundaryNoiseRemoval.h"
#include "ImageData.h"
#include "ConvertImage1D.h"
#include "ConvertImage3D.h"

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
	bool upsampleImage(int iLeft, ImageData<ImageType> *pcYuv);	//!> Set up the reference pictures
	bool DoViewInterpolation(ImageData<ImageType>& pYuvBuffer);	//!> The main interface function to be called to perform view interpolation

	ImageData<DepthType> getDepthMapLeft() { return m_depthMapLeft; }
	ImageData<DepthType> getDepthMapRight() { return m_depthMapRight; }
	int getFrameNumber() { return m_iFrameNumber; }	//Zhejiang

	void setFrameNumber(int frame_number) { m_iFrameNumber = frame_number; }

private:
	bool xViewSynthesis3D(ImageData<ImageType>& pYuvBuffer);
	bool xViewSynthesis1D(ImageData<ImageType>& pSynYuvBuffer);
	bool xBoundaryNoiseRemoval(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight, ImageData<HoleType>& pRefHoleLeft, ImageData<HoleType>& pRefHoleRight, ImageData<ImageType>& pSynYuvBuffer, bool SynthesisMode);
	
private:
	void initInputImages();
	void initOutputImages();

	ConfigSyn& cfg;

	int m_iFrameNumber;		     //used in TIM, Zhejiang

	int m_height;	//!> Source height
	int m_width;	//!> Source width
	int m_width2;	//!> The width when the ref view is upsampled (may be 2*Width or 4*Width, it depends). otherwise, it is the same as Width

	ImageData<DepthType> m_depthMapLeft;		//!> To store the depth map of the left reference view
	ImageData<DepthType> m_depthMapRight;		//!> To store the depth map of the right reference view
	ImageData<ImageType> m_imageLeft;			//!> To store the image of the left reference view: (w*precision, h)
	ImageData<ImageType> m_imageRight;			//!> To store the image of the right reference view: (w*precision, h)

	ImageData<ImageType> m_tempImageLeft;			//!> input read (w, h) to upsample
	ImageData<ImageType> m_tempImageRight;		//!> input read (w, h) to upsample

	ViewSynthesis3D m_viewSynthesis3D;			//!> The object to do 3D warping view synthesis
	ViewSynthesis1D m_viewSynthesis1D;			//!> The object to do 1D view synthesis
	BoundaryNoiseRemoval m_boundaryNoiseRemoval;	//!> The object to do boundary noise removal

	ConvertImage* m_convertImage;

	// result
	unsigned char* m_synColorLeft;
	unsigned char* m_synColorRight;
	unsigned char* m_synDepthLeft;
	unsigned char* m_synDepthRight;

};

#endif
