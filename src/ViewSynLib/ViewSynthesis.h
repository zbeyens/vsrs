#ifndef _INCLUDE_VIEW_INTERPOLATION_H_
#define _INCLUDE_VIEW_INTERPOLATION_H_

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "ViewSynthesisGeneral.h"
#include "ViewSynthesis1D.h"
#include "BoundaryNoiseRemoval.h"
#include "yuv.h"

/*
 * Interface class between application and synthesis objects
 */
class ViewSynthesis
{
public:
	// init fields to NULL
	ViewSynthesis();
	virtual ~ViewSynthesis();

	bool    Init();
	bool    SetReferenceImage(int iLeft, ImageYuv<ImageType> *pcYuv);//!< Set up the reference pictures
	bool    DoViewInterpolation(ImageYuv<ImageType>* pYuvBuffer);    //!< The main interface function to be called to perform view interpolation

	ImageYuv<DepthType>*    getDepthBufferLeft() { return m_pcDepthMapLeft; }
	ImageYuv<DepthType>*    getDepthBufferRight() { return m_pcDepthMapRight; }
	int	      getFrameNumber() { return m_iFrameNumber; }	//Zhejiang

	void      setFrameNumber(int frame_number) { m_iFrameNumber = frame_number; }

private:
	bool    xViewSynthesisGeneralMode(ImageYuv<ImageType>* pYuvBuffer);
	bool    xViewSynthesis1DMode(ImageYuv<ImageType>* pSynYuvBuffer);
	bool    xBoundaryNoiseRemoval(ImageYuv<ImageType>* pRefLeft, ImageYuv<ImageType>* pRefRight, ImageYuv<DepthType>* pRefDepthLeft, ImageYuv<DepthType>* pRefDepthRight, ImageYuv<HoleType>* pRefHoleLeft, ImageYuv<HoleType>* pRefHoleRight, ImageYuv<ImageType>* pSynYuvBuffer, bool SynthesisMode);
	void    xFileConvertingforGeneralMode(ImageYuv<ImageType>* pRefLeft, ImageYuv<ImageType>* pRefRight, ImageYuv<DepthType>* pRefDepthLeft, ImageYuv<DepthType>* pRefDepthRight, ImageYuv<HoleType>* pRefHoleLeft, ImageYuv<HoleType>* pRefHoleRight);
	void    xFileConvertingfor1DMode(ImageYuv<ImageType>* pRefLeft, ImageYuv<ImageType>* pRefRight, ImageYuv<DepthType>* pRefDepthLeft, ImageYuv<DepthType>* pRefDepthRight, ImageYuv<HoleType>* pRefHoleLeft, ImageYuv<HoleType>* pRefHoleRight);

private:
	void initInputImages();
	void initOutputImages();

	ConfigSyn& cfg;

	//? tbd
	int     m_iFrameNumber;		     //used in TIM, Zhejiang

	int            mWidth;	//!< Source width * precision
	int            mHeight;	//!< Source height

	ImageYuv<DepthType>* m_pcDepthMapLeft;		//!< To store the depth map of the left reference view
	ImageYuv<DepthType>* m_pcDepthMapRight;	//!< To store the depth map of the right reference view
	ImageYuv<ImageType>* m_pcImageLeft;		//!< To store the image of the left reference view
	ImageYuv<ImageType>* m_pcImageRight;		//!< To store the image of the right reference view

	ImageYuv<ImageType>* m_pcTempYuvLeft;		//!< src w/h
	ImageYuv<ImageType>* m_pcTempYuvRight;		//!< src w/h

	ViewSynthesisGeneral* m_pViewSynthesisGeneral;	//!< The object to do 3D warping view synthesis
	ViewSynthesis1D* m_pViewSynthesis1D;			//!< The object to do 1D view synthesis
	BoundaryNoiseRemoval* m_pBoundaryNoiseRemoval;	//!< The object to do boundary noise removal

	// result
	unsigned char* m_pSynColorLeft;
	unsigned char* m_pSynColorRight;
	unsigned char* m_pSynDepthLeft;
	unsigned char* m_pSynDepthRight;

#ifdef _DEBUG
	//? tbd
	unsigned char    m_ucSetup;
#endif
};

#endif
