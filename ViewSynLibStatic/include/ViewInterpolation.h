#ifndef _INCLUDE_VIEW_INTERPOLATION_H_
#define _INCLUDE_VIEW_INTERPOLATION_H_

#ifndef _OPEN_CV_HEADERS_
#define _OPEN_CV_HEADERS_
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cvaux.h>
#endif

#include "yuv.h"

class CParameterViewInterpolation;      // Class for view synthesis parameters
class CViewInterpolationGeneral;        // Class for general mode
class CViewInterpolation1D;             // Class for 1-D view synthesis
class CBoundaryNoiseRemoval;            // Class for boundary noise removal

/*
 * Interface class between application and sysnthesis objects
 */

class CViewInterpolation
{
public:
	// init fields to NULL
	CViewInterpolation();
	virtual ~CViewInterpolation();

	/*!
		Get the parameters needed from cParameter
		to continue...
	*/
	bool    Init(CParameterViewInterpolation& cParameter);
	bool    SetReferenceImage(int iLeft, CIYuv<ImageType> *pcYuv);//!< Set up the reference pictures
	bool    DoViewInterpolation(CIYuv<ImageType>* pYuvBuffer);    //!< The main interface function to be called to perform view interpolation

	CIYuv<DepthType>*    getDepthBufferLeft() { return m_pcDepthMapLeft; }
	CIYuv<DepthType>*    getDepthBufferRight() { return m_pcDepthMapRight; }
	unsigned int  getBoundaryNoiseRemoval() { return  m_uiBoundary; }
	Int	      getFrameNumber() { return m_iFrameNumber; }	//Zhejiang
	void      setFrameNumber(int frame_number) { m_iFrameNumber = frame_number; }

private:
	bool    xViewInterpolationGeneralMode(CIYuv<ImageType>* pYuvBuffer);
	bool    xViewInterpolation1DMode(CIYuv<ImageType>* pSynYuvBuffer);
	bool    xBoundaryNoiseRemoval(CIYuv<ImageType>* pRefLeft, CIYuv<ImageType>* pRefRight, CIYuv<DepthType>* pRefDepthLeft, CIYuv<DepthType>* pRefDepthRight, CIYuv<HoleType>* pRefHoleLeft, CIYuv<HoleType>* pRefHoleRight, CIYuv<ImageType>* pSynYuvBuffer, bool SynthesisMode);
	void    xFileConvertingforGeneralMode(CIYuv<ImageType>* pRefLeft, CIYuv<ImageType>* pRefRight, CIYuv<DepthType>* pRefDepthLeft, CIYuv<DepthType>* pRefDepthRight, CIYuv<HoleType>* pRefHoleLeft, CIYuv<HoleType>* pRefHoleRight);
	void    xFileConvertingfor1DMode(CIYuv<ImageType>* pRefLeft, CIYuv<ImageType>* pRefRight, CIYuv<DepthType>* pRefDepthLeft, CIYuv<DepthType>* pRefDepthRight, CIYuv<HoleType>* pRefHoleLeft, CIYuv<HoleType>* pRefHoleRight);

private:
	// Default
	Int     m_iSynthesisMode; //!< from CParameterViewInterpolation
	//? tbd
	Int     m_iFrameNumber;		     //used in TIM, Zhejiang

	unsigned int    m_uiColorSpace;		//!< from CParameterViewInterpolation
	unsigned int    m_uiViewBlending;	//!< from CParameterViewInterpolation
	unsigned int    m_uiBoundary;		//!< from CParameterViewInterpolation

	CIYuv<DepthType>*          m_pcDepthMapLeft;	//!< To store the depth map of the left reference view, from CParameterViewInterpolation
	CIYuv<DepthType>*          m_pcDepthMapRight;	//!< To store the depth map of the right reference view, from CParameterViewInterpolation
	CIYuv<ImageType>*          m_pcImageLeft;		//!< To store the image of the left reference view, from CParameterViewInterpolation
	CIYuv<ImageType>*          m_pcImageRight;		//!< To store the image of the right reference view, from CParameterViewInterpolation
	//? tbd
	CIYuv<ImageType>*          m_pcTempYuvLeft;
	CIYuv<ImageType>*          m_pcTempYuvRight;

	CViewInterpolationGeneral* m_pViewSynthesisGeneral;	//!< The object to do 3D warping view synthesis
	CViewInterpolation1D*      m_pViewSynthesis1D;      //!< The object to do 1D view synthesis
	CBoundaryNoiseRemoval*     m_pBoundaryNoiseRemoval; //!< The object to do boundary noise removal

	Int            m_iWidth;	//!< Source width
	Int            m_iHeight;	//!< Source height

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
