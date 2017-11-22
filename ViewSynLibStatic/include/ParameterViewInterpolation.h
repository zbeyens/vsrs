//======================================created by Ying Chen =====================================
//===============Tampere University of Technology (TUT)/Nokia Research Center (NRC)===============

#ifndef AFX_PARAMETER_VIEW_INTERPOLATION_H
#define AFX_PARAMETER_VIEW_INTERPOLATION_H

#include "iostream"
#include "string"

#include "ParameterBase.h"
#include "version.h"

//using namespace std;


/*
 */
 /*!
	\brief
		Store the parameters of one camera
	
	All camera parameters are estimated with calibration software, used by DERS and VSRS

	For each camera section:
	-the first 3 lines give the camera intrinsics (focal length, principal point,...)
	-the last 3 lines give the camera extrinsics (camera rotation and translation)

	 references: CParameterViewInterpolation
 */
class CCameraParameters
{
public:
	double m_fIntrinsicMatrix[3][3];	//!< Intrinsic parameters
	double m_fExtrinsicMatrix[3][3];	//!< Extrinsic parameters
	double m_fTranslationVector[3];		//!< Translation vector

	/*
		init:
		-3x3 intrinsic matrix
		-1x3 translation vector
		-3x3 extrinsic matrix
	*/
	CCameraParameters();
	// release
	~CCameraParameters() {};

	//assign the 3 matrix from another CCameraParameters
	CCameraParameters& operator = (CCameraParameters& src);
};

/*
 * Read parameter values from config file
 */
class CParameterViewInterpolation : public ParameterBase
{

public:
	//todo useless to init values: it is done by setup()
	/*
		init parameters to default values
	*/
	CParameterViewInterpolation();
	virtual ~CParameterViewInterpolation();

	/*
		\brief
		Initialize the parameters to default value
		Parse config parameter file and set given values
		Print params
		Release m_pCfgLines as it is no longer needed
		Parse camera parameter file and set given values
		Check the parameters validation

		\param argc
		\param argv config filename

		\return
		-1: Fail if no arg
		 1: Succeed
	*/
	Int     Init(Int argc, Char** argv);

	//! Setters never used, it is modified from setup reference 
	Void    setDepthType(UInt ui) { m_uiDepthType = ui; }
	Void    setSourceWidth(UInt ui) { m_uiSourceWidth = ui; }
	Void    setSourceHeight(UInt ui) { m_uiSourceHeight = ui; }
	Void    setNumberOfFrames(UInt ui) { m_uiNumberOfFrames = ui; }
	Void    setStartFrame(UInt ui) { m_uiStartFrame = ui; }

	Void    setLeftNearestDepthValue(Double d) { m_dLeftNearestDepthValue = d; }
	Void    setLeftFarthestDepthValue(Double d) { m_dRightFarthestDepthValue = d; }
	Void    setRightNearestDepthValue(Double d) { m_dLeftNearestDepthValue = d; }
	Void    setRightFarthestDepthValue(Double d) { m_dRightFarthestDepthValue = d; }

	Void    setCameraParameterFile(std::string s) { m_cCameraParameterFile = s; }

	Void    setLeftCameraName(std::string s) { m_cLeftCameraName = s; }
	Void    setVirtualCameraName(std::string s) { m_cVirtualCameraName = s; }
	Void    setRightCameraName(std::string s) { m_cRightCameraName = s; }

	Void    setLeftViewImageName(std::string s) { m_cLeftViewImageName = s; }
	Void    setRightViewImageName(std::string s) { m_cRightViewImageName = s; }
	Void    setLeftDepthMapName(std::string s) { m_cLeftDepthMapName = s; }
	Void    setRightDepthMapName(std::string s) { m_cRightDepthMapName = s; }
	//Void    setVirtualViewImageName    ( std::string s )      { m_cVirtualViewImageName  =  s;  }

	Void    setOutputVirViewImageName(std::string s) { m_cOutputVirViewImageName = s; }
	Void    setColorSpace(UInt ui) { m_uiColorSpace = ui; }
	Void    setPrecision(UInt ui) { m_uiPrecision = ui; }
	Void    setFilter(UInt ui) { m_uiFilter = ui; }

#ifdef NICT_IVSRS
	// NICT start  
	Void    setIvsrsInpaint(UInt ui) { m_uiIvsrsInpaint = ui; }
	// NICT end
#endif

	Void    setSynthesisMode(UInt ui) { m_uiSynthesisMode = ui; }

	UInt    setBoundaryNoiseRemoval(UInt ui) { m_uiBoundaryNoiseRemoval = ui; }

	Void    setViewBlending(UInt ui) { m_uiViewBlending = ui; }

#ifdef POZNAN_DEPTH_BLEND
	Void    setDepthBlendDiff(Int i) { m_iDepthBlendDiff = i; };
#endif

	UInt    getDepthType() { return m_uiDepthType; }
	UInt    getSourceWidth() { return m_uiSourceWidth; }
	UInt    getSourceHeight() { return m_uiSourceHeight; }
	UInt    getNumberOfFrames() { return m_uiNumberOfFrames; }
	UInt    getStartFrame() { return m_uiStartFrame; }

	Double  getLeftNearestDepthValue() { return m_dLeftNearestDepthValue; }
	Double  getLeftFarthestDepthValue() { return m_dLeftFarthestDepthValue; }
	Double  getRightNearestDepthValue() { return m_dRightNearestDepthValue; }
	Double  getRightFarthestDepthValue() { return m_dRightFarthestDepthValue; }

	const std::string   getCameraParameterFile() { return m_cCameraParameterFile; }

	const std::string   getLeftCameraName() { return m_cLeftCameraName; }
	const std::string   getVirtualCameraName() { return m_cVirtualCameraName; }
	const std::string   getRightCameraName() { return m_cRightCameraName; }

	const std::string   getLeftViewImageName() { return m_cLeftViewImageName; }
	const std::string   getRightViewImageName() { return m_cRightViewImageName; }
	const std::string   getLeftDepthMapName() { return m_cLeftDepthMapName; }
	const std::string   getRightDepthMapName() { return m_cRightDepthMapName; }
	//const std::string   getVirtualViewImageName    ()   { return m_cVirtualViewImageName;  }

	const std::string   getOutputVirViewImageName() { return m_cOutputVirViewImageName; }

	//! For 1D mode
	Double getFocalLength() { return m_camParam[0].m_fIntrinsicMatrix[0][0]; } //!< Get the focal length from the left camera. We assume all the three cameras share the same focal length
	Double getLTranslationLeft() { return m_camParam[1].m_fTranslationVector[0] - m_camParam[0].m_fTranslationVector[0]; }//!< Tx(Syn) - Tx(Left)
	Double getLTranslationRight() { return m_camParam[1].m_fTranslationVector[0] - m_camParam[2].m_fTranslationVector[0]; } //!< Tx(Syn) - Tx(Right)
	Double getduPrincipalLeft() { return m_camParam[1].m_fIntrinsicMatrix[0][2] - m_camParam[0].m_fIntrinsicMatrix[0][2]; } //!< uxSyn - uxLeft;
	Double getduPrincipalRight() { return m_camParam[1].m_fIntrinsicMatrix[0][2] - m_camParam[2].m_fIntrinsicMatrix[0][2]; } //!< uxSyn - uxRight;

	Double *getMat_Ex_Left() { return &m_camParam[0].m_fExtrinsicMatrix[0][0]; }
	Double *getMat_Ex_Virtual() { return &m_camParam[1].m_fExtrinsicMatrix[0][0]; }
	Double *getMat_Ex_Right() { return &m_camParam[2].m_fExtrinsicMatrix[0][0]; }
	Double *getMat_In_Left() { return &m_camParam[0].m_fIntrinsicMatrix[0][0]; }
	Double *getMat_In_Virtual() { return &m_camParam[1].m_fIntrinsicMatrix[0][0]; }
	Double *getMat_In_Right() { return &m_camParam[2].m_fIntrinsicMatrix[0][0]; }
	Double *getMat_Trans_Left() { return &m_camParam[0].m_fTranslationVector[0]; }
	Double *getMat_Trans_Virtual() { return &m_camParam[1].m_fTranslationVector[0]; }
	Double *getMat_Trans_Right() { return &m_camParam[2].m_fTranslationVector[0]; }

	//? not used?
	Double getLeftBaselineDistance() { return m_dLeftBaselineDistance; }
	Double getRightBaselineDistance() { return m_dRightBaselineDistance; }

	// Access algorithm parameter for 1D mode
	Int    getSplattingOption() { return m_iSplattingOption; }
	//todo typo
	Int    getBoudaryGrowth() { return m_iBoundaryGrowth; }
	Int    getMergingOption() { return m_iMergingOption; }
	Int    getDepthThreshold() { return m_iDepthThreshold; }
	Int    getHoleCountThreshold() { return m_iHoleCountThreshold; }
	Int    getTemporalImprovementOption() { return m_iTemporalImprovementOption; } //Zhejiang, May,4
	Int    getWarpEnhancementOption() { return m_iWarpEnhancementOption; }
	Int    getCleanNoiseOption() { return m_iCleanNoiseOption; }

	UInt    getColorSpace() { return m_uiColorSpace; }

	UInt    getPrecision() { return m_uiPrecision; }
	UInt    getFilter() { return m_uiFilter; }

#ifdef NICT_IVSRS
	// NICT start  
	UInt    getIvsrsInpaint() { return m_uiIvsrsInpaint; }
	// NICT end
#endif

	UInt    getSynthesisMode() { return m_uiSynthesisMode; }
	UInt    getBoundaryNoiseRemoval() { return m_uiBoundaryNoiseRemoval; }
	UInt    getViewBlending() { return m_uiViewBlending; }

#ifdef POZNAN_DEPTH_BLEND
	Int     getDepthBlendDiff() { return m_iDepthBlendDiff; }
#endif

private:
	/*!
		Fill the parameter config lines array (helper to parse config file)
		Set all parameter values to a default value

		\return
		# of parameter lines
	*/
	UInt    setup();

	/*!
	* \brief
	*    Read the camera parameters from file to memory

		Scan the file till reaching the id of one of the 3 camera parameters (given in the config file)
		Scan the values and store them in the 2 matrix and translation vector
		Repeat for the 3 cameras
	* \return
	*    1: Succeed
	*    0: Fail
	*/
	//todo divide the function
	//todo order of camera indexes
	UInt    xReadCameraParameters();

	/*
	* \brief
	*    Check whether the config parameters are valid or not

		For 1D mode:
		-display Warning if the world coordinate system is used (results may be incorrect).
		-display Error if the input format is RGB.
	* \return
	*    1: Succeed
	*    0: Fail
	*/
	//todo no error
	UInt    xValidation();
	Double m_dLeftBaselineDistance;
	Double m_dRightBaselineDistance;

protected:
	Int     xPrintUsage(Char**  argv);

protected:
	//? difference
	/*!
		Specifies the depth type.
		The input value 0 means the view synthesis mode by using depth from a camera.
		The input value 1 means the view synthesis mode by using depth from the origin of 3D space.
		Default: 0
	*/
	UInt            m_uiDepthType;
	//! Input frame width. Default: 0
	UInt            m_uiSourceWidth;
	//! Input frame height. Default: 0
	UInt            m_uiSourceHeight;
	//! Total number of input frame. Default: 0
	UInt            m_uiNumberOfFrames;
	//! Starting frame #. Default: 0
	UInt            m_uiStartFrame;

	//! Nearest depth value of left image from camera or the origin of 3D space. Default: 0
	//? why
	Double          m_dLeftNearestDepthValue;
	//! Farthest depth value of left image from camera or the origin of 3D space. Default: 0
	Double          m_dLeftFarthestDepthValue;
	//! Nearest depth value of right image from camera or the origin of 3D space. Default: 0
	Double          m_dRightNearestDepthValue;
	//! Farthest depth value of right image from camera or the origin of 3D space. Default: 0
	Double          m_dRightFarthestDepthValue;


	//! Name of text file which includes real and virtual camera parameters
	std::string     m_cCameraParameterFile;
	//! Name of real left camera
	std::string     m_cLeftCameraName;
	//! Name of real right camera
	std::string     m_cRightCameraName;
	//! Name of virtual camera
	std::string     m_cVirtualCameraName;

	//! Name of left input video
	std::string     m_cLeftViewImageName;
	//! Name of left depth map video
	std::string     m_cLeftDepthMapName;
	//! Name of right input video
	std::string     m_cRightViewImageName;
	//! Name of right depth map video
	std::string     m_cRightDepthMapName;

	//std::string     m_cVirtualViewImageName;
	//! Name of output virtual view video
	std::string     m_cOutputVirViewImageName;

	//! 0...YUV, 1...RGB
	//? difference
	UInt      m_uiColorSpace;

	//! 1...Integer-pel, 2...Half-pel, 4...Quater-pel
	//? meaning
	UInt      m_uiPrecision;

	//! 0...(Bi)-linear, 1...(Bi)-Cubic, 2...MPEG-4 AVC
	//? meaning

	UInt      m_uiFilter;

#ifdef NICT_IVSRS
	// NICT start  
	//! Type of inpaing used 0...conventional 1...NICT improved one. Default: 1
	//? not in cfg
	UInt    m_uiIvsrsInpaint;
	// NICT end
#endif
	//! 0...General, 1...1D parallel
	UInt      m_uiSynthesisMode;
	//! 0: No Boundary Noise Removal, 1 : Use Boundary Noise Removal
	UInt      m_uiBoundaryNoiseRemoval;
	//! 0...Blend left and right images, 1...Not Blend
	UInt      m_uiViewBlending;

#ifdef POZNAN_DEPTH_BLEND
	//? not in cfg
	Int       m_iDepthBlendDiff;
#endif

	// Algorithm parameters for 1-D view synthesis mode
	//! 0: No upsampling for ref pictures; 1: Upsample ref pictures  >>>> Hide this parameter from config file
	//Int  m_iUpsampleRefs;        

	//! 0: Disable splatting; 1: Enable splatting for all pixels; 2: Splatting only along boundaries. Default: 2
	Int  m_iSplattingOption;
	//! Only useful for SplattingOption 2 - A parameter to enlarge the boundary area with SplattingOption = 2. Default: 40
	Int  m_iBoundaryGrowth;
	//! 0: Z_buffer, 1: camera distance weighting. 2: Z_buffer + hole counting + camera distance weighting. Default: 2
	Int  m_iMergingOption;
	//! Only useful for MergingOption 2. Range: 0 ~ 255. Default: 75
	Int  m_iDepthThreshold;
	//! Only useful for MergingOption 2. Range: 0 ~ 255. Default: 30
	Int  m_iHoleCountThreshold;
	//! 0: Disable; 1; Enable. Default: 1    Zhejiang,May,4
	//? not in cfg
	Int  m_iTemporalImprovementOption;
	//! 0: Disable; 1: Enable. Default: 0
	//? not in cfg
	Int  m_iWarpEnhancementOption;
	//! 0: Disable; 1; Enable. Default: 0
	//? not in cfg
	Int  m_iCleanNoiseOption;

	// Camera parameters
	//! 0: Left, 1: Center, 2: Right
	CCameraParameters m_camParam[3];
};

#endif 
