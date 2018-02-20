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
	bool    SetReferenceImage(int iLeft, CIYuv<ImageType> *pcYuv);//!< Set up the reference pictures
	bool    DoViewInterpolation(CIYuv<ImageType>* pYuvBuffer);    //!< The main interface function to be called to perform view interpolation

	CIYuv<DepthType>*    getDepthBufferLeft() { return m_pcDepthMapLeft; }
	CIYuv<DepthType>*    getDepthBufferRight() { return m_pcDepthMapRight; }
	int	      getFrameNumber() { return m_iFrameNumber; }	//Zhejiang

	void      setFrameNumber(int frame_number) { m_iFrameNumber = frame_number; }

private:
	bool    xViewSynthesisGeneralMode(CIYuv<ImageType>* pYuvBuffer);
	bool    xViewSynthesis1DMode(CIYuv<ImageType>* pSynYuvBuffer);
	bool    xBoundaryNoiseRemoval(CIYuv<ImageType>* pRefLeft, CIYuv<ImageType>* pRefRight, CIYuv<DepthType>* pRefDepthLeft, CIYuv<DepthType>* pRefDepthRight, CIYuv<HoleType>* pRefHoleLeft, CIYuv<HoleType>* pRefHoleRight, CIYuv<ImageType>* pSynYuvBuffer, bool SynthesisMode);
	void    xFileConvertingforGeneralMode(CIYuv<ImageType>* pRefLeft, CIYuv<ImageType>* pRefRight, CIYuv<DepthType>* pRefDepthLeft, CIYuv<DepthType>* pRefDepthRight, CIYuv<HoleType>* pRefHoleLeft, CIYuv<HoleType>* pRefHoleRight);
	void    xFileConvertingfor1DMode(CIYuv<ImageType>* pRefLeft, CIYuv<ImageType>* pRefRight, CIYuv<DepthType>* pRefDepthLeft, CIYuv<DepthType>* pRefDepthRight, CIYuv<HoleType>* pRefHoleLeft, CIYuv<HoleType>* pRefHoleRight);

private:
	void initInputImages();
	void initOutputImages();

	ConfigSyn& cfg;

	//? tbd
	int     m_iFrameNumber;		     //used in TIM, Zhejiang

	int            mWidth;	//!< Source width
	int            mHeight;	//!< Source height

	CIYuv<DepthType>*          m_pcDepthMapLeft;	//!< To store the depth map of the left reference view, from ConfigSyn
	CIYuv<DepthType>*          m_pcDepthMapRight;	//!< To store the depth map of the right reference view, from ConfigSyn
	CIYuv<ImageType>*          m_pcImageLeft;		//!< To store the image of the left reference view, from ConfigSyn
	CIYuv<ImageType>*          m_pcImageRight;		//!< To store the image of the right reference view, from ConfigSyn
	//? tbd
	CIYuv<ImageType>*          m_pcTempYuvLeft;
	CIYuv<ImageType>*          m_pcTempYuvRight;

	ViewSynthesisGeneral* m_pViewSynthesisGeneral;	//!< The object to do 3D warping view synthesis
	ViewSynthesis1D*      m_pViewSynthesis1D;      //!< The object to do 1D view synthesis
	BoundaryNoiseRemoval*     m_pBoundaryNoiseRemoval; //!< The object to do boundary noise removal

	//? tbd - output?
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
