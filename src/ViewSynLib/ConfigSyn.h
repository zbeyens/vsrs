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

	uint getDepthType();
	int getSourceWidth();
	int getSourceHeight();
	uint getNumberOfFrames();
	uint getStartFrame();

	double getLeftNearestDepthValue();
	double getLeftFarthestDepthValue();
	double getRightNearestDepthValue();
	double getRightFarthestDepthValue();

	const string getCameraParameterFile();

	const string getLeftCameraName();
	const string getVirtualCameraName();
	const string getRightCameraName();

	const string getLeftViewImageName();
	const string getRightViewImageName();
	const string getLeftDepthMapName();
	const string getRightDepthMapName();
	//const string getVirtualViewImageName ();}

	const string getOutputVirViewImageName();

	// For 1D mode
	double getFocalLength();		//!< Get the focal length from the left camera. We assume all the three cameras share the same focal length
	double getTranslationXLeft();	//!< Tx(Syn) - Tx(Left)
	double getTranslationXRight();	//!< Tx(Syn) - Tx(Right)
	double getPrincipalXLeft();		//!< uxSyn - uxLeft;
	double getPrincipalXRight();	//!< uxSyn - uxRight;

									//!> For general mode
	double* getMat_Rot_Left();
	double* getMat_Rot_Virtual();
	double* getMat_Rot_Right();
	double* getMat_In_Left();
	double* getMat_In_Virtual();
	double* getMat_In_Right();
	double* getMat_Trans_Left();
	double* getMat_Trans_Virtual();
	double* getMat_Trans_Right();

	//? not used?
	double getLeftBaselineDistance();
	double getRightBaselineDistance();

	// Access algorithm parameter for 1D mode
	int getSplattingOption();
	int getBoundaryGrowth();
	int getMergingOption();
	int getDepthThreshold();
	int getHoleCountThreshold();
	int getTemporalImprovementOption(); //Zhejiang, May,4
	int getWarpEnhancementOption();
	int getCleanNoiseOption();

	uint getColorSpace();

	uint getPrecision();
	uint getFilter();

	uint getIvsrsInpaint();

	uint getSynthesisMode();
	uint getBoundaryNoiseRemoval();
	uint getViewBlending();

	int  getDepthBlendDiff();


	ConfigCam* getConfigCams();

private:
	ConfigSyn();
	~ConfigSyn();

	//? not used?
	double m_dLeftBaselineDistance;
	double m_dRightBaselineDistance;

	//? difference
	/*!
	Specifies the depth type.
	The input value 0 means the view synthesis mode by using depth from a camera.
	The input value 1 means the view synthesis mode by using depth from the origin of 3D space.
	Default: 0
	*/
	uint m_uiDepthType;
	int m_uiSourceWidth;				//!> Input frame width. Default: 0
	int m_uiSourceHeight;				//!> Input frame height. Default: 0
	uint m_uiNumberOfFrames;			//!> Total number of input frame. Default: 0
	uint m_uiStartFrame;				//!> Starting frame #. Default: 0
	
	//? why
	double m_dLeftNearestDepthValue;	//!> Nearest depth value of left image from camera or the origin of 3D space. Default: 0
	double m_dLeftFarthestDepthValue;	//!> Farthest depth value of left image from camera or the origin of 3D space. Default: 0
	double m_dRightNearestDepthValue;	//!> Nearest depth value of right image from camera or the origin of 3D space. Default: 0
	double m_dRightFarthestDepthValue;	//!> Farthest depth value of right image from camera or the origin of 3D space. Default: 0
	
	string m_cCameraParameterFile;		//!> Name of text file which includes real and virtual camera parameters
	string m_cLeftCameraName;			//!> Name of real left camera
	string m_cRightCameraName;			//!> Name of real right camera
	string m_cVirtualCameraName;		//!> Name of virtual camera
	
	string m_cLeftViewImageName;		//!> Name of left input video
	string m_cLeftDepthMapName;			//!> Name of left depth map video
	string m_cRightViewImageName;		//!> Name of right input video
	string m_cRightDepthMapName;		//!> Name of right depth map video
	string m_cOutputVirViewImageName;	//!> Name of output virtual view video

	//? difference
	uint m_uiColorSpace;				//!> 0...YUV, 1...RGB

	//? meaning
	uint m_uiPrecision;					//!> 1...Integer-pel, 2...Half-pel, 4...Quater-pel
	//? meaning
	uint m_uiFilter;					//!> 0...(Bi)-linear, 1...(Bi)-Cubic, 2...MPEG-4 AVC
	//? not in cfg
	uint m_uiIvsrsInpaint;				//!> Type of inpaing used 0...conventional 1...NICT improved one. Default: 1

	uint m_uiSynthesisMode;				//!> 0...General, 1...1D parallel
	uint m_uiBoundaryNoiseRemoval;		//!> 0: No Boundary Noise Removal, 1 : Use Boundary Noise Removal
	uint m_uiViewBlending;				//!> 0...Blend left and right images, 1...Not Blend
	

	//? not in cfg
	int m_iDepthBlendDiff;

	// Algorithm parameters for 1-D view synthesis mode
	
	int m_iSplattingOption;				//!> 0: Disable splatting; 1: Enable splatting for all pixels; 2: Splatting only along boundaries. Default: 2
	int m_iBoundaryGrowth;				//!> Boundary dialation, useful for Splatting option 2.  A parameter to enlarge the boundary area. Default: 40
	int m_iMergingOption;				//!> 0: Z_buffer, 1: camera distance weighting. 2: Z_buffer + hole counting + camera distance weighting. Default: 2
	int m_iDepthThreshold;				//!> Only useful for MergingOption 2. Range: 0 ~ 255. Default: 75	
	int m_iHoleCountThreshold;			//!> Only useful for MergingOption 2. Range: 0 ~ 255. Default: 30
	int m_iTemporalImprovementOption;	//!> 0: Disable; 1; Enable. Default: 1    Zhejiang,May,4
	int m_iWarpEnhancementOption;		//!> 0: Disable; 1: Enable. Default: 0
	int m_iCleanNoiseOption;			//!> 0: Disable; 1; Enable. Default: 0
	
	//int m_bitDepth;

	// Camera parameters
	//!> 0: Left, 1: Center, 2: Right
	ConfigCam m_camParam[3];
};