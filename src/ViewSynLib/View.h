#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "Camera.h"
#include "Tools.h"
#include "Image.h"
#include "Homography.h"

/*!
	Synthesis from one view (i.e left, right,...)
*/
class View
{
public:
	View();
	~View();


	/*!
		Init ViewSynthesis fields for one sample

		@param	indexCam	index of the camera
	*/
	bool init(uint indexCam);

	/*!
		Copy warping results for BNR
	*/
	bool convertMatToBuffer1D();

	IplImage*  getSynHolesMat() { return m_synHoles->getMat(); }
	IplImage*  getSynFillsMat() { return m_synFills->getMat(); }
	IplImage*  getSynImageMat() { return m_synImage->getMat(); }
	IplImage*  getSynDepthMat() { return m_synDepth->getMat(); }
	IplImage*  getUnstablePixelsMat() { return m_imgMask[0]->getMat(); }

	shared_ptr<Image<ImageType>> getImageNotUpsampled() { return m_imageNotUpsampled; }
	shared_ptr<Image<ImageType>> getImageHorUpsampled() { return m_imageHorUpsampled; }
	shared_ptr<Image<ImageType>> getImage() { return m_image; }
	shared_ptr<Image<DepthType>> getDepth() { return m_depth; }
	shared_ptr<Image<DepthType>> getSynDepth() { return m_synDepth; }
	shared_ptr<Image<ImageType>> getSynImage() { return m_synImage; }
	shared_ptr<Image<HoleType>> getSynFills() { return m_synFills; }
	shared_ptr<Image<HoleType>>  getSynHoles() { return m_synHoles; }
	shared_ptr<Image<HoleType>>  getFillableHoles() { return m_fillableHoles; }

	double getWeight() { return m_baselineWeight; }

	double getInvZNearMinusInvZFar() { return m_invZNearMinusInvZFar; }
	double getInvZFar() { return m_invZfar; }
	double* getTableD2Z() { return m_tableD2Z; }
	CvMat* getMatH_R2V() { return m_matH_R2V; }
	CvMat* getMatH_V2R() { return m_matH_V2R; }
	shared_ptr<Camera> getCam() { return cam; }

	shared_ptr<Image<HoleType>> getImgMask(int i) { return m_imgMask[i]; }

	void setWeight(double weight) { m_baselineWeight = weight; }

private:
	void    xReleaseMemory();

	/*!
		Init all the intermediary images
	*/
	bool initImages();
	/*!
		Init 3D warping parameters from the camera parameters. Apply the homography.
	*/
	bool init_3Dwarp();

	/*!
		Calculate the depth of the view depending on the config
	*/
	bool computeDepth();
	/*!
		Compute the depth from the camera origin
	*/
	void computeDepthFromCam(int i, double distance);
	/*!
		Compute the depth from the space origin
	*/
	void computeDepthFromSpace(int i, double distance);

	ConfigSyn& cfg = ConfigSyn::getInstance();

	shared_ptr<Camera> cam; //!> for computations depending on the camera parameters

	uint m_indexView; //!< Index of the View. Needed for the parsed config

	shared_ptr<Image<ImageType>> m_imageNotUpsampled;	//!> input read (w, h) not upsampled, stored in BGR or YUV format
	shared_ptr<Image<ImageType>> m_imageHorUpsampled;	//!> image horizontally upsampled
	shared_ptr<Image<ImageType>> m_image;				//!> To store the image of the reference views: (w*precision, h)
	shared_ptr<Image<DepthType>> m_depth;				//!> To store the depth map of the reference views: (w*precision, h)

	shared_ptr<Image<ImageType>> m_synImage;	//!< for viewReverse warping: warped image
	shared_ptr<Image<DepthType>> m_synDepth;	//!< for warping: warped depth map
	shared_ptr<Image<HoleType>> m_synFills;	//!< for warping - non-holes (0 if hole, 255 else)

	shared_ptr<Image<HoleType>> m_synHoles;			//!< for warping - holes (255 if hole, 0 else)

	shared_ptr<Image<ImageType>> m_fillableHoles;	//!> holes fillable by any other view fills

	shared_ptr<Image<HoleType>> m_imgMask[2];	//!< for warping: [0] unstable pixels, [1]

	Homography m_homography; //!> compute the homography of the view using the camera parameters

	CvMat*  m_matH_R2V;	//!< for depth warping - homography real to virtual
	CvMat*  m_matH_V2R;	//!< for viewReverse warping - homography virtual to real

	double m_invZNearMinusInvZFar;	//!< 1/zNear - 1/zFar
	double m_invZfar;				//!< 1/zFar
	double m_tableD2Z[MAX_DEPTH]; //!< for warping - Depth to Z

	double m_baselineWeight; //!> depending on the camera baselines
};