#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "Camera.h"
#include "Tools.h"
#include "Image.h"
#include "Homography.h"

class Warp;

/**
Synthesis from one view (i.e left, right)
*/
class View
{
public:
	View();
	~View();

	void    xReleaseMemory();

	/**
	Init ViewSynthesis fields for one sample

	@param	indexCam		index of the camera. 0 = left, 1 = right
	*/
	bool Init(uint indexCam);

	bool    xSynthesizeView(ImageType ***src, DepthType **pDepthMap, int th_same_depth = 5);

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

	Image<DepthType> getImgVirtualDepth() { return m_imgVirtualDepth; }
	Image<ImageType> getImgVirtualImage() { return m_imgVirtualImage; }
	Image<HoleType> getImgSuccessSynthesis() { return m_imgSuccessSynthesis; }
	Image<HoleType> getImgHoles() { return m_imgHoles; }
	double* getTableD2Z() { return m_dTableD2Z; }
	CvMat* getMatH_R2V() { return m_matH_R2V; }
	CvMat* GetMatH_V2R() { return m_matH_V2R; }
	Camera getCam() { return cam; }

	//public to private:

	double m_dInvZNearMinusInvZFar;	//!< POZNAN_DEPTH_PROJECT2COMMON
	double m_dInvZfar;				//!< POZNAN_DEPTH_PROJECT2COMMON
	Image<HoleType> m_imgMask[2];	//!< for warping

	Image<HoleType> m_imgBound;	//!< for viewReverse warping

	Camera cam;

	Image<ImageType> m_imgVirtualImage;	//!< for viewReverse warping
	Image<DepthType> m_imgVirtualDepth;	//!< for warping
	Image<HoleType> m_imgSuccessSynthesis;	//!< for warping
	Image<HoleType> m_imgHoles; //!< for warping

	CvMat*  m_matH_R2V;	//!< for depth warping
	CvMat*  m_matH_V2R;	//!< for viewReverse warping

	double m_dTableD2Z[MAX_DEPTH]; //!< for warping

private:
	bool initIntermImages();
	bool init_3Dwarp();

	void computeDepthFromCam();
	void computeDepthFromSpace();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	uint mIndexSyn; //!< Index of the ViewSynthesis. 0 = left, 1 = right

	Homography m_homography;

	Warp* m_viewSynthesisReverse;
	Warp* m_depthSynthesis;
};