#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "Camera.h"
#include "Tools.h"
#include "Image.h"

/**
	Synthesis from one view (i.e left, right)
*/
class Sample
{
public:
	Sample();
	~Sample();

	void    xReleaseMemory();

	/**
	Init ViewSynthesis fields for one sample

	@param	indexCam		index of the camera. 0 = left, 1 = right
	*/
	bool Init(uint indexCam);

	bool    xSynthesizeView(ImageType ***src, DepthType **pDepthMap, int th_same_depth = 5);
	bool    xSynthesizeDepth(DepthType **pDepthMap, ImageType ***src);
	bool    xSynthesizeView_reverse(ImageType ***src, DepthType **pDepthMap, int th_same_depth = 5);

	IplImage*  getHolePixels() { return m_imgHoles.getImageIpl(); }
	IplImage*  getSynthesizedPixels() { return m_imgSuccessSynthesis.getImageIpl(); }
	IplImage*  getVirtualImage() { return m_imgVirtualImage.getImageIpl(); }
	IplImage*  getVirtualDepthMap() { return m_imgVirtualDepth.getImageIpl(); }
	IplImage*  getUnstablePixels() { return m_imgMask[0].getImageIpl(); }
	//Nagoya start
	ImageType**    getVirtualImageY() { return m_imgVirtualImage.getImageY(); }
	ImageType**    getVirtualImageU() { return m_imgVirtualImage.getImageU(); }
	ImageType**    getVirtualImageV() { return m_imgVirtualImage.getImageV(); }
	//Nagoya end
	DepthType**    getVirtualDepth() { return m_imgVirtualDepth.getImageY(); }
	//HoleType**    getNonHoles() { return m_imgSuccessSynthesis.getImageY(); }

	// GIST added
	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* GetMatH_V2R() { return m_matH_V2R; }

	Camera getCam() { return cam; }

private:
	bool initIntermImages();
	void initSynthesisFunctions();
	bool    init_3Dwarp();
	void    makeHomography(CvMat *&matH_F2T, CvMat *&matH_T2F, double adTable[MAX_DEPTH], CvMat *matIN_from, CvMat *matEX_c2w_from, CvMat *matProj_to);

	void    cvexMedian(IplImage* dst);
	void    cvexBilateral(IplImage* dst, int sigma_d, int sigma_c);
	void    erodebound(IplImage* bound, int flag);

	bool    depthsynthesis_3Dwarp(DepthType **pDepthMap, ImageType ***src);
	bool    depthsynthesis_3Dwarp_ipel(DepthType **pDepthMap, ImageType ***src);

	bool    viewsynthesis_reverse_3Dwarp(ImageType ***src, DepthType **pDepthMap, int th_same_depth);
	bool    viewsynthesis_reverse_3Dwarp_ipel(ImageType ***src, DepthType **pDepthMap, int th_same_depth);

	//not used:
	int      median_filter_depth(IplImage *srcDepth, IplImage *dstDepth, IplImage *srcMask, IplImage *dstMask, int sizeX, int sizeY, bool bSmoothing);
	int      median_filter_depth_wCheck(IplImage *srcDepth, IplImage *dstDepth, IplImage *srcMask, IplImage *dstMask, int sizeX, int sizeY, bool bSmoothing, int th_same_plane = 5);
	void    image2world_with_z(CvMat *mat_Rc2w_invIN_from, CvMat *matEX_c2w_from, CvMat *image, CvMat *world);

	ConfigSyn& cfg = ConfigSyn::getInstance();

	//Camera cameras[2];
	Camera cam;

	uint mIndexSyn; //!< Index of the ViewSynthesis. 0 = left, 1 = right

	Image<ImageType> m_imgVirtualImage;
	Image<DepthType> m_imgVirtualDepth;
	Image<HoleType> m_imgSuccessSynthesis;
	Image<HoleType> m_imgHoles;
	Image<HoleType> m_imgBound;
	Image<HoleType> m_imgMask[2];
	Image<ImageType> m_imgTemp[2];
	Image<DepthType> m_imgDepthTemp[2];

	IplConvKernel*    m_pConvKernel;

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat*  m_matH_R2V;
	CvMat*  m_matH_V2R;

	double m_dTableD2Z[MAX_DEPTH];

	double m_dInvZNearMinusInvZFar;	//!< POZNAN_DEPTH_PROJECT2COMMON
	double m_dInvZfar;				//!< POZNAN_DEPTH_PROJECT2COMMON

	int*  m_aiTableDisparity_ipel;
	int*  m_aiTableDisparity_subpel;

	//! ViewSynthesisReverse function depending on the precision
	bool (Sample::*m_pFunc_ViewSynthesisReverse) (ImageType ***src, DepthType **pDepthMap, int th_same_depth);
	//! DepthSynthesis function depending on the precision
	bool (Sample::*m_pFunc_DepthSynthesis) (DepthType **pDepthMap, ImageType ***src);
};