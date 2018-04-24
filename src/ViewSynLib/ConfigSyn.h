#pragma once

#include "SystemIncludes.h"
#include "ConfigCam.h"
#include "Singleton.h"

#define VERSION 4.3

#define IMAGE_DEPTH				8

#define IMAGE_CHANNELS			3
#define BLENDED_DEPTH_CHANNELS	1
#define DEPTH_CHANNELS			1
#define MASK_CHANNELS			1

/**
444: full resolution
422: full vert, 1/2 hor resolution
420: 1/2 vert, 1/2 hor resolution
*/
#define DEPTHMAP_CHROMA_FORMAT	420	//!< Depth map YUV format
#define HOLE_CHROMA_FORMAT		420 //!< Hole YUV format
#define HOLE1D_CHROMA_FORMAT	400 //!< Hole YUV format
#define IMAGE_CHROMA_FORMAT		444	//!< Image YUV format

#define BUFFER_CHROMA_FORMAT 420

#define MAX_LUMA 256
#define MAX_HOLE 256
#define MAX_DEPTH (256*256)
typedef unsigned char ImageType;
typedef unsigned char HoleType;

#define POZNAN_16BIT_DEPTH        // original: comment out, 8bit
#ifdef POZNAN_16BIT_DEPTH
typedef unsigned short DepthType;
#define MAX_DEPTH (256*256)

#else
typedef unsigned char DepthType;
#define MAX_DEPTH 256
#endif

#define BYTE unsigned char
#define WORD unsigned short

#define uint unsigned int
typedef unsigned char uchar;

template <typename T>  __inline  int   MaxTypeValue() { return 0; };
template <>               __inline  int   MaxTypeValue <unsigned short>() { return (256 * 256); };
template <>               __inline  int   MaxTypeValue <unsigned char>() { return (256); };

#define cvmMul( src1, src2, dst )       cvMatMulAdd( src1, src2, 0, dst )
#define cvmCopy( src, dst )             cvCopy( src, dst, 0 )
#define cvmInvert( src, dst )           cvInv( src, dst )

/* 1D definitions */
//hole filling related
#define HOLEPIXEL    1
#define NOTHOLEPIXEL 0

//depth edge detection related, Zhejiang Univ.
#define NONE    0
#define RISE    1 //the top point of a rising edge
#define FALL    2 //the top point of a falling edge
#define AROUND  3 //the area around edge to be used in Boundary-Aware Splatting
#define SUSPECT 4 //pixel do not be warped for the sake of cleaning boundary noise

// temporal improvement related
#define TEMPOBLOCK3 0  //1(ON) temporal block =3, 0(OFF): =5;

//Zmap using and LDV bg and bd generation
#define USE_ZMAP     1      // Enable for now until the depth map can be generated with identical Znear and Zfar for the left and right views
#define USE_LDV      0      // Enable for highlight the occlusion layer extraction code

// The index number
#define LEFTVIEW  0
#define RGHTVIEW  1
#define MERGVIEW  2
#define HLFLVIEW  3
#define FINLVIEW  4

//Zhejiang, DEBUG test info
#define DEBUG_ZHEJIANG 0
#define VSRS3_ORIGINAL 0    // DT: This switch may be removed after London meeting.
/* end 1D definitions */

class ConfigSyn : public Singleton<ConfigSyn>
{
	friend Singleton<ConfigSyn>;

public:
	const int MODE_3D = 0;
	const int MODE_1D = 1;
	const int DEPTH_FROM_CAMERA = 0;
	const int DEPTH_FROM_ORIGIN = 1;
	const int BLEND_ALL = 0;
	const int BLEND_CLOSER = 1;
	const int FILTER_LINEAR = 0;
	const int FILTER_CUBIC = 1;
	const int FILTER_AVC = 2;
	const int COLOR_SPACE_YUV = 0;
	const int COLOR_SPACE_RGB = 1;
	const int INPAINT_DEFAULT = 0;
	const int INPAINT_ADVANCED = 1;
	const int BNR_DISABLED = 0;
	const int BNR_ENABLED = 1;

	/*
	\brief
	Check whether the config parameters are valid or not

	For 1D mode:
	-display Warning if the world coordinate system is used (results may be incorrect).
	-display Error if the input format is RGB.

	\return
	1: Succeed
	0: Fail
	*/
	uint validation();

	void printParams(); //!< Print all parameters

	void setParams(map<string, string> params);

	uint getNViews() { return m_nViews; }

	uint getDepthType();
	int getSourceWidth();
	int getSourceHeight();
	uint getNumberOfFrames();
	uint getStartFrame();

	double getNearestDepthValue(int i) { return m_nearestDepthValues[i]; }
	double getFarthestDepthValue(int i) { return m_farthestDepthValues[i]; }
	const string getCameraName(int i) { return m_cameraNames[i]; }
	const string getViewImageName(int i) { return m_viewImageNames[i]; }
	const string getDepthMapName(int i) { return m_depthMapNames[i]; }

	const string getCameraParameterFile();

	const string getVirtualCameraName();

	const string getOutputVirViewImageName();

	// For 1D mode
	double getFocalLength() { return m_camParams[0]->mIntrinsicMatrix[0][0]; }	//!< Get the focal length from the left camera. We assume all the three cameras share the same focal length
	double getTranslationXLeft() { return m_virtualConfigCam->mTranslationVector[0] - m_camParams[0]->mTranslationVector[0]; }	//!< Tx(Syn) - Tx(Left)
	double getTranslationXRight() { return m_virtualConfigCam->mTranslationVector[0] - m_camParams[1]->mTranslationVector[0]; }
	double getPrincipalXLeft() { return m_virtualConfigCam->mIntrinsicMatrix[0][2] - m_camParams[0]->mIntrinsicMatrix[0][2]; }	//!< uxSyn - uxLeft;
	double getPrincipalXRight() { return m_virtualConfigCam->mIntrinsicMatrix[0][2] - m_camParams[1]->mIntrinsicMatrix[0][2]; }

	//For 3D mode
	double* getMat_Rot_Virtual() { return m_virtualConfigCam->getRotationMatrix(); }
	double* getMat_In_Virtual() { return m_virtualConfigCam->getIntrinsicMatrix(); }
	double* getMat_Trans_Virtual() { return m_virtualConfigCam->getTranslationVector(); }
	double* getMat_Rot(int i) { return m_camParams[i]->getRotationMatrix(); }
	double* getMat_In(int i) { return m_camParams[i]->getIntrinsicMatrix(); }
	double* getMat_Trans(int i) { return m_camParams[i]->getTranslationVector(); }

	// Access algorithm parameter for 1D mode
	int getSplattingOption() { return m_splattingOption; }
	int getBoundaryGrowth() { return m_boundaryGrowth; }
	int getMergingOption() { return m_mergingOption; }
	int getDepthThreshold() { return m_depthThreshold; }
	int getHoleCountThreshold() { return m_holeCountThreshold; }
	int getTemporalImprovementOption() { return m_temporalImprovementOption; } //Zhejiang, May,4
	int getWarpEnhancementOption() { return m_warpEnhancementOption; }
	int getCleanNoiseOption() { return m_cleanNoiseOption; }

	uint getColorSpace() { return m_colorSpace; }

	uint getPrecision() { return m_precision; }
	uint getFilter() { return m_filter; }

	uint getIvsrsInpaint() { return m_IvsrsInpaint; }

	uint getSynthesisMode() { return m_synthesisMode; }
	uint getBoundaryNoiseRemoval() { return m_boundaryNoiseRemoval; }
	uint getViewBlending() { return m_viewBlending; }

	int  getDepthBlendDiff() { return m_depthBlendDiff; }

	ConfigCam* getConfigCam(int i) { return m_camParams[i]; }
	vector<ConfigCam*> getConfigCams() { return m_camParams; }
	ConfigCam* getVirtualConfigCam() { return m_virtualConfigCam; }

	uint getTesting() { return m_testing; }
	string getTestImageName() { return m_testImageName; }

private:
	ConfigSyn();
	~ConfigSyn();

	void initViewsParams();

	uint m_testing;
	string m_testImageName;

	/*!
	Specifies the depth type.
	The input value 0 means the view synthesis mode by using depth from a camera.
	The input value 1 means the view synthesis mode by using depth from the origin of 3D space.
	Default: 0
	*/
	uint m_depthType;
	int m_sourceWidth;				//!> Input frame width. Default: 0
	int m_sourceHeight;				//!> Input frame height. Default: 0
	uint m_numberOfFrames;			//!> Total number of input frame. Default: 0
	uint m_startFrame;				//!> Starting frame #. Default: 0

	uint m_nViews;

	vector<double> m_nearestDepthValues;	//!> Nearest depth values from camera or the origin of 3D space. Default: 0
	vector<double> m_farthestDepthValues;	//!> Farthest depth value from camera or the origin of 3D space. Default: 0

	string m_cameraParameterFile;		//!> Name of text file which includes real and virtual camera parameters
	vector<string> m_cameraNames;		//!> Name of real cameras
	string m_virtualCameraName;			//!> Name of virtual camera

	vector<string> m_viewImageNames;		//!> Name of images video
	vector<string> m_depthMapNames;			//!> Name of depth maps video
	string m_outputVirViewImageName;	//!> Name of output virtual view video

	uint m_colorSpace;				//!> 0...YUV, 1...RGB format for intermerdiary computations

	uint m_precision;					//!> 1...Integer-pel, 2...Half-pel, 4...Quater-pel for the filters
	uint m_filter;					//!> 0...(Bi)-linear, 1...(Bi)-Cubic, 2...MPEG-4 AVC for the upsampling
	uint m_IvsrsInpaint;				//!> Type of inpaing used 0...conventional 1...NICT improved one. Default: 1

	uint m_synthesisMode;				//!> 0...General, 1...1D parallel
	uint m_boundaryNoiseRemoval;		//!> 0: No Boundary Noise Removal, 1 : Use Boundary Noise Removal
	uint m_viewBlending;				//!> 0...Blend left and right images, 1...Not Blend

	int m_depthBlendDiff;				//!> max depth difference to blend left and right pixel using weighted baseline. If bigger, the nearer pixel is chosen.

	// Algorithm parameters for 1-D view synthesis mode

	int m_splattingOption;				//!> 0: Disable splatting; 1: Enable splatting for all pixels; 2: Splatting only along boundaries. Default: 2
	int m_boundaryGrowth;				//!> Boundary dialation, useful for Splatting option 2.  A parameter to enlarge the boundary area. Default: 40
	int m_mergingOption;				//!> 0: Z_buffer, 1: camera distance weighting. 2: Z_buffer + hole counting + camera distance weighting. Default: 2
	int m_depthThreshold;				//!> Only useful for MergingOption 2. Range: 0 ~ 255. Default: 75
	int m_holeCountThreshold;			//!> Only useful for MergingOption 2. Range: 0 ~ 255. Default: 30
	int m_temporalImprovementOption;	//!> 0: Disable; 1; Enable. Default: 1    Zhejiang,May,4
	int m_warpEnhancementOption;		//!> 0: Disable; 1: Enable. Default: 0
	int m_cleanNoiseOption;			//!> 0: Disable; 1; Enable. Default: 0

	//int m_bitDepth;

	vector<ConfigCam*> m_camParams;		//!> Camera parameters of the views
	ConfigCam* m_virtualConfigCam;		//!> Camera parameters of the virtual camera
};