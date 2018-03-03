#ifndef _VIEW_SYNTHESIS_H_
#define _VIEW_SYNTHESIS_H_

#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4819)

#include "SystemIncludes.h"
#include "yuv.h"
#include "ConfigSyn.h"
#include "Tools.h"
#include "View.h"
#include "Image.h"

/**
* View Synthesis using general mode
*/
class ViewSynthesisGeneral
{
public:
	ViewSynthesisGeneral();
	~ViewSynthesisGeneral();

	void    xReleaseMemory();

	/**
	  Init Left and Right Sample
	*/
	bool InitLR();

	int  DoOneFrameGeneral(ImageType*** RefLeft, ImageType*** RefRight, DepthType** RefDepthLeft, DepthType** RefDepthRight, ImageYuv<ImageType>* pSynYuvBuffer);

	IplImage*  getImgSynthesizedViewLeft() { return m_pcViewSynthesisLeft->getVirtualImage(); }
	IplImage*  getImgSynthesizedViewRight() { return m_pcViewSynthesisRight->getVirtualImage(); }

	View* GetInterpolatedLeft() { return m_pcViewSynthesisLeft; }
	View* GetInterpolatedRight() { return m_pcViewSynthesisRight; }
	IplImage*  GetSynLeftWithHole() { return m_imgSynLeftforBNR; }
	IplImage*  GetSynRightWithHole() { return m_imgSynRightforBNR; }
	IplImage*  GetSynDepthLeftWithHole() { return m_imgDepthLeftforBNR; }
	IplImage*  GetSynDepthRightWithHole() { return m_imgDepthRightforBNR; }
	IplImage*  GetSynHoleLeft() { return m_imgHoleLeftforBNR; }
	IplImage*  GetSynHoleRight() { return m_imgHoleRightforBNR; }

	//TODO
	//Nagoya start
	ImageType* GetSynColorLeftY() { return *(m_pcViewSynthesisLeft->getVirtualImageY()); }
	ImageType* GetSynColorRightY() { return *(m_pcViewSynthesisRight->getVirtualImageY()); }
	ImageType* GetSynColorLeftU() { return *(m_pcViewSynthesisLeft->getVirtualImageU()); }
	ImageType* GetSynColorRightU() { return *(m_pcViewSynthesisRight->getVirtualImageU()); }
	ImageType* GetSynColorLeftV() { return *(m_pcViewSynthesisLeft->getVirtualImageV()); }
	ImageType* GetSynColorRightV() { return *(m_pcViewSynthesisRight->getVirtualImageV()); }
	DepthType* GetSynDepthLeft() { return *(m_pcViewSynthesisLeft->getVirtualDepth()); }
	DepthType* GetSynDepthRight() { return *(m_pcViewSynthesisRight->getVirtualDepth()); }
	//Nagoya end

private:

	void computeWeightLR(); //!< Compute weight left and right from baseline left and right
	void initResultImages();	//!< Create all the cv images for synthesis

	View*  m_pcViewSynthesisLeft;
	View*  m_pcViewSynthesisRight;

	ConfigSyn& cfg;

	double        m_dWeightLeft;
	double        m_dWeightRight;
	double        WeightLeft;
	double        WeightRight;

	Image<ImageType> m_imgBlended;          //!> Blended image
	Image<DepthType> m_imgBlendedDepth;     //!> Blended depth // NICT
	Image<ImageType>  m_imgInterpolatedView; //!> The final image buffer to be output
	Image<ImageType>      m_imgMask[3];

	double  LeftBaselineDistance;
	double  RightBaselineDistance;

	// GIST added
	IplImage*      m_imgSynLeftforBNR;
	IplImage*      m_imgSynRightforBNR;
	IplImage*      m_imgDepthLeftforBNR;
	IplImage*      m_imgDepthRightforBNR;
	IplImage*      m_imgHoleLeftforBNR;
	IplImage*      m_imgHoleRightforBNR;
	// GIST end

#ifdef _DEBUG
	unsigned char    m_ucSetup;
#endif
};

#endif
