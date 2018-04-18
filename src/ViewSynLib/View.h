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
	bool init(uint indexCam);

	bool synthesizeView();

	IplImage*  getSynHolesMat() { return m_synHoles->getMat(); }
	IplImage*  getSynFillsMat() { return m_synFills->getMat(); }
	IplImage*  getSynImageMat() { return m_synImage->getMat(); }
	IplImage*  getSynDepthMat() { return m_synDepth->getMat(); }
	IplImage*  getUnstablePixelsMat() { return m_imgMask[0].getMat(); }

	Image<ImageType>* getImageNotUpsampled() { return m_imageNotUpsampled; }
	Image<ImageType>* getImage() { return m_image; }
	Image<DepthType>* getDepth() { return m_depth; }
	Image<DepthType>* getSynDepth() { return m_synDepth; }
	Image<ImageType>* getSynImage() { return m_synImage; }
	Image<HoleType>* getSynFills() { return m_synFills; }
	Image<ImageType>*  getSynImageWithHole() { return m_synImageWithHole; }
	Image<DepthType>*  getSynDepthWithHole() { return m_synDepthWithHole; }
	Image<HoleType>*  getSynHoles() { return m_synHoles; }
	Image<HoleType>*  getFillableHoles() { return m_fillableHoles; }

	double getWeight() { return m_baselineWeight; }

	double* getTableD2Z() { return m_tableD2Z; }
	CvMat* getMatH_R2V() { return m_matH_R2V; }
	CvMat* getMatH_V2R() { return m_matH_V2R; }
	Camera getCam() { return cam; }

	void setWeight(double weight) { m_baselineWeight = weight; }

	//public to private:

	double m_invZNearMinusInvZFar;	//!< 1/zNear - 1/zFar
	double m_invZfar;				//!< 1/zFar
	Image<HoleType> m_imgMask[2];	//!< for warping: [0] unstable pixels, [1]

	Image<HoleType> m_imgBound;	//!< for viewReverse warping

	CvMat*  m_matH_R2V;	//!< for depth warping - homography real to virtual
	CvMat*  m_matH_V2R;	//!< for viewReverse warping - homography virtual to real

	double m_tableD2Z[MAX_DEPTH]; //!< for warping

private:
	bool initIntermImages();
	bool init_3Dwarp();

	bool computeDepth();
	void computeDepthFromCam(int i, double distance);
	void computeDepthFromSpace(int i, double distance);

	ConfigSyn& cfg = ConfigSyn::getInstance();

	Camera cam;

	uint m_indexView; //!< Index of the ViewSynthesis. 0 = left, 1 = right. Needed for the parsed config

	Homography m_homography;

	Warp* m_viewSynthesisReverse;
	Warp* m_depthSynthesis;

	double m_baselineWeight;

	Image<ImageType>* m_imageNotUpsampled;	//!> input read (w, h) not upsampled, stored in BGR or YUV format
	Image<ImageType>* m_image;				//!> To store the image of the reference views: (w*precision, h)
	Image<DepthType>* m_depth;				//!> To store the depth map of the reference views: (w*precision, h)

	Image<ImageType>* m_synImage;	//!< for viewReverse warping: warped image
	Image<DepthType>* m_synDepth;	//!< for warping: warped depth map
	Image<HoleType>* m_synFills;	//!< for warping - non-holes (0 if hole, 255 else)

	Image<ImageType>* m_synImageWithHole;		//!> output of the warping for BNR
	Image<DepthType>* m_synDepthWithHole;
	Image<HoleType>* m_synHoles;			//!< for warping - holes (255 if hole, 0 else)

	Image<ImageType>* m_fillableHoles;	//!> holes fillable by any other view fills

};