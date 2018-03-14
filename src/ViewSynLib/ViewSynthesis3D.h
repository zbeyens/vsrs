#ifndef _VIEW_SYNTHESIS_H_
#define _VIEW_SYNTHESIS_H_

#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4819)

#include "SystemIncludes.h"
#include "ImageData.h"
#include "ConfigSyn.h"
#include "View.h"
#include "Image.h"
#include "ImageTools.h"

/**
* View Synthesis using general mode
*/
class ViewSynthesis3D
{
public:
	ViewSynthesis3D();
	~ViewSynthesis3D();

	void    xReleaseMemory();

	/**
	  Init Left and Right View
	*/
	bool Init();

	bool  apply(ImageType*** RefLeft, ImageType*** RefRight, DepthType** RefDepthLeft, DepthType** RefDepthRight, ImageData<ImageType>& pSynYuvBuffer);

	IplImage*  getImgSynthesizedViewLeft() { return m_viewLeft->getVirtualImageIpl(); }
	IplImage*  getImgSynthesizedViewRight() { return m_viewRight->getVirtualImageIpl(); }

	View* GetViewLeft() { return m_viewLeft; }
	View* GetViewRight() { return m_viewRight; }
	IplImage*  GetSynLeftWithHole() { return m_imgSynLeftforBNR; }
	IplImage*  GetSynRightWithHole() { return m_imgSynRightforBNR; }
	IplImage*  GetSynDepthLeftWithHole() { return m_imgDepthLeftforBNR; }
	IplImage*  GetSynDepthRightWithHole() { return m_imgDepthRightforBNR; }
	IplImage*  GetSynHoleLeft() { return m_imgHoleLeftforBNR; }
	IplImage*  GetSynHoleRight() { return m_imgHoleRightforBNR; }

	ImageType* GetSynColorLeftY() { return *(m_viewLeft->getVirtualImageY()); }
	ImageType* GetSynColorRightY() { return *(m_viewRight->getVirtualImageY()); }
	ImageType* GetSynColorLeftU() { return *(m_viewLeft->getVirtualImageU()); }
	ImageType* GetSynColorRightU() { return *(m_viewRight->getVirtualImageU()); }
	ImageType* GetSynColorLeftV() { return *(m_viewLeft->getVirtualImageV()); }
	ImageType* GetSynColorRightV() { return *(m_viewRight->getVirtualImageV()); }
	DepthType* GetSynDepthLeft() { return *(m_viewLeft->getVirtualDepthY()); }
	DepthType* GetSynDepthRight() { return *(m_viewRight->getVirtualDepthY()); }

private:

	void computeWeightLR(); //!< Compute weight left and right from baseline left and right
	void initResultImages();	//!< Create all the cv images for synthesis

	void Blending();
	void NoBlending();

	View*  m_viewLeft;
	View*  m_viewRight;

	ConfigSyn& cfg;

	double        m_weightLeft;
	double        m_weightRight;
	double        WeightLeft;
	double        WeightRight;

	Image<ImageType> m_imgBlended;          //!> Blended image
	Image<DepthType> m_imgBlendedDepth;     //!> Blended depth
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

};

#endif
