#include "ConfigSyn.h"

ConfigSyn::ConfigSyn()
	: m_uiDepthType(0)
	, m_uiSourceWidth(1280)
	, m_uiSourceHeight(960)
	, m_uiStartFrame(0)
	, m_uiNumberOfFrames(300)
	, m_dLeftNearestDepthValue(0.0)
	, m_dLeftFarthestDepthValue(0.0)
	, m_dRightNearestDepthValue(0.0)
	, m_dRightFarthestDepthValue(0.0)
	, m_cCameraParameterFile("cam_param.txt")
	, m_cLeftCameraName("param_dog38")
	, m_cRightCameraName("param_dog39")
	, m_cVirtualCameraName("param_dog41")
	, m_cLeftViewImageName("dog038.yuv")
	, m_cLeftDepthMapName("dog041.yuv")
	, m_cRightViewImageName("depth_dog038.yuv")
	, m_cRightDepthMapName("depth_dog041.yuv")
	, m_cOutputVirViewImageName("dog_virtual039.yuv")
	//, m_cVirtualViewImageName		()
	, m_uiColorSpace(0)
	, m_uiPrecision(2)
	, m_uiFilter(1)
	, m_uiSynthesisMode(0)
	, m_uiBoundaryNoiseRemoval(0)
	, m_uiViewBlending(0)
	, m_iDepthBlendDiff(5)
	, m_uiIvsrsInpaint(1)
	, m_iSplattingOption(2)
	, m_iBoundaryGrowth(40)
	, m_iMergingOption(2)
	, m_iDepthThreshold(75)
	, m_iHoleCountThreshold(30)
	, m_iTemporalImprovementOption(1)
	, m_iWarpEnhancementOption(1)
	, m_iCleanNoiseOption(1)
{
}

ConfigSyn::~ConfigSyn()
{
	//release();
}

void ConfigSyn::setParams(map<string, string> params)
{
	for (auto const& param : params)
	{
		string tag = param.first;
		string value = param.second;
		//cout << param.first << ':' << param.second << endl;

		if (tag == "DepthType") m_uiDepthType = atoi(value.c_str());
		else if (tag == "SourceWidth") m_uiSourceWidth = atoi(value.c_str());
		else if (tag == "SourceHeight") m_uiSourceHeight = atoi(value.c_str());
		else if (tag == "TotalNumberOfFrames") m_uiNumberOfFrames = atoi(value.c_str());
		else if (tag == "StartFrame") m_uiStartFrame = atoi(value.c_str());

		else if (tag == "LeftNearestDepthValue") m_dLeftNearestDepthValue = atof(value.c_str());
		else if (tag == "LeftFarthestDepthValue") m_dLeftFarthestDepthValue = atof(value.c_str());
		else if (tag == "RightNearestDepthValue") m_dRightNearestDepthValue = atof(value.c_str());
		else if (tag == "RightFarthestDepthValue") m_dRightFarthestDepthValue = atof(value.c_str());

		else if (tag == "CameraParameterFile") m_cCameraParameterFile = value;
		else if (tag == "LeftCameraName") m_cLeftCameraName = value;
		else if (tag == "VirtualCameraName") m_cVirtualCameraName = value;
		else if (tag == "RightCameraName") m_cRightCameraName = value;
		else if (tag == "LeftViewImageName") m_cLeftViewImageName = value;
		else if (tag == "RightViewImageName") m_cRightViewImageName = value;
		else if (tag == "LeftDepthMapName") m_cLeftDepthMapName = value;
		else if (tag == "RightDepthMapName") m_cRightDepthMapName = value;
		//else if (tag == "ReferenceVirtualViewImageName") m_cVirtualViewImageName = value;
		else if (tag == "OutputVirtualViewImageName") m_cOutputVirViewImageName = value;

		else if (tag == "ColorSpace") m_uiColorSpace = atoi(value.c_str());
		else if (tag == "Precision") m_uiPrecision = atoi(value.c_str());
		else if (tag == "Filter") m_uiFilter = atoi(value.c_str());
		else if (tag == "SynthesisMode") m_uiSynthesisMode = atoi(value.c_str());
		else if (tag == "BoundaryNoiseRemoval") m_uiBoundaryNoiseRemoval = atoi(value.c_str());
		else if (tag == "ViewBlending") m_uiViewBlending = atoi(value.c_str());
		else if (tag == "DepthBlendDifference") m_iDepthBlendDiff = atoi(value.c_str());
		else if (tag == "IvsrsInpaint") m_uiIvsrsInpaint = atoi(value.c_str());
		else if (tag == "SplattingOption") m_iSplattingOption = atoi(value.c_str());
		else if (tag == "BoundaryGrowth") m_iBoundaryGrowth = atoi(value.c_str());
		else if (tag == "MergingOption") m_iMergingOption = atoi(value.c_str());
		else if (tag == "DepthThreshold") m_iDepthThreshold = atoi(value.c_str());
		else if (tag == "HoleCountThreshold") m_iHoleCountThreshold = atoi(value.c_str());
		else if (tag == "TemporalImprovementOption") m_iTemporalImprovementOption = atoi(value.c_str());
		else if (tag == "WarpEnhancementOption") m_iWarpEnhancementOption = atoi(value.c_str());
		else if (tag == "CleanNoiseOption") m_iCleanNoiseOption = atoi(value.c_str());
	}
}

void ConfigSyn::printParams()
{
	cout << "DepthType : " << m_uiDepthType << endl;
	cout << "SourceWidth : " << m_uiSourceWidth << endl;
	cout << "SourceHeight : " << m_uiSourceHeight << endl;
	cout << "TotalNumberOfFrames : " << m_uiNumberOfFrames << endl;
	cout << "StartFrame : " << m_uiStartFrame << endl;

	cout << "LeftNearestDepthValue : " << m_dLeftNearestDepthValue << endl;
	cout << "LeftFarthestDepthValue : " << m_dLeftFarthestDepthValue << endl;
	cout << "RightNearestDepthValue : " << m_dRightNearestDepthValue << endl;
	cout << "RightFarthestDepthValue : " << m_dRightFarthestDepthValue << endl;

	cout << "CameraParameterFile : " << m_cCameraParameterFile << endl;
	cout << "LeftCameraName : " << m_cLeftCameraName << endl;
	cout << "VirtualCameraName : " << m_cVirtualCameraName << endl;
	cout << "RightCameraName : " << m_cRightCameraName << endl;
	cout << "LeftViewImageName : " << m_cLeftViewImageName << endl;
	cout << "RightViewImageName : " << m_cRightViewImageName << endl;
	cout << "LeftDepthMapName : " << m_cLeftDepthMapName << endl;
	cout << "RightDepthMapName : " << m_cRightDepthMapName << endl;
	//cout << "ReferenceVirtualViewImageName : " << m_cVirtualViewImageName << endl;
	cout << "OutputVirtualViewImageName : " << m_cOutputVirViewImageName << endl;

	cout << "ColorSpace : " << m_uiColorSpace << endl;
	cout << "Precision : " << m_uiPrecision << endl;
	cout << "Filter : " << m_uiFilter << endl;
	cout << "SynthesisMode : " << m_uiSynthesisMode << endl;
	cout << "BoundaryNoiseRemoval : " << m_uiBoundaryNoiseRemoval << endl;
	cout << "ViewBlending : " << m_uiViewBlending << endl;
	cout << "DepthBlendDifference : " << m_iDepthBlendDiff << endl;
	cout << "IvsrsInpaint : " << m_uiIvsrsInpaint << endl;
	cout << "SplattingOption : " << m_iSplattingOption << endl;
	cout << "BoundaryGrowth : " << m_iBoundaryGrowth << endl;
	cout << "MergingOption : " << m_iMergingOption << endl;
	cout << "DepthThreshold : " << m_iDepthThreshold << endl;
	cout << "HoleCountThreshold : " << m_iHoleCountThreshold << endl;
	cout << "TemporalImprovementOption : " << m_iTemporalImprovementOption << endl;
	cout << "WarpEnhancementOption : " << m_iWarpEnhancementOption << endl;
	cout << "CleanNoiseOption : " << m_iCleanNoiseOption << endl;
}

uint ConfigSyn::validation()
{
	if (m_uiPrecision != 1 && m_uiPrecision != 2 && m_uiPrecision != 4)
	{
		fprintf(stderr, "Illegal Precision setting\n");
		return VSRS_ERROR;
	}

	if (m_uiSynthesisMode == 1) // 1D mode
	{
		if (m_uiDepthType == 1)
		{
			cout << "Warning: The depth levels must be measured against the cameras. If the world coordinate system does not" << endl;
			cout << "originate from the cameras, synthesis results may be incorrect." << endl;
		}
		if (m_uiColorSpace != 0)
		{
			cout << "Error: The input format must be in YUV 420 with 1D synthesis mode, for the time being." << endl;
			cout << "       Check ColorSpace." << endl;
			return VSRS_ERROR;
		}

		if (m_uiBoundaryNoiseRemoval && m_iSplattingOption != 1) {
			if (m_iSplattingOption != 1) {
				cout << endl << "Warning: When you use 1D Mode, 'Boundary Noise Removal' mode supports only" << endl;
				cout << "         only 'SplattingOption = 1'." << endl;
				cout << "         Hence, 'SplattingOption' is changed to '1'." << endl << endl;
				m_iSplattingOption = 1;
			}
		}
	}

	// More checks for general mode:
	if (m_uiSynthesisMode == 0) // general mode
	{
	}

	return VSRS_OK;
}

uint ConfigSyn::getDepthType() { return m_uiDepthType; }
int ConfigSyn::getSourceWidth() { return m_uiSourceWidth; }
int ConfigSyn::getSourceHeight() { return m_uiSourceHeight; }
uint ConfigSyn::getNumberOfFrames() { return m_uiNumberOfFrames; }
uint ConfigSyn::getStartFrame() { return m_uiStartFrame; }

double ConfigSyn::getLeftNearestDepthValue() { return m_dLeftNearestDepthValue; }
double ConfigSyn::getLeftFarthestDepthValue() { return m_dLeftFarthestDepthValue; }
double ConfigSyn::getRightNearestDepthValue() { return m_dRightNearestDepthValue; }
double ConfigSyn::getRightFarthestDepthValue() { return m_dRightFarthestDepthValue; }

const string ConfigSyn::getCameraParameterFile() { return m_cCameraParameterFile; }

const string ConfigSyn::getLeftCameraName() { return m_cLeftCameraName; }
const string ConfigSyn::getVirtualCameraName() { return m_cVirtualCameraName; }
const string ConfigSyn::getRightCameraName() { return m_cRightCameraName; }

const string ConfigSyn::getLeftViewImageName() { return m_cLeftViewImageName; }
const string ConfigSyn::getRightViewImageName() { return m_cRightViewImageName; }
const string ConfigSyn::getLeftDepthMapName() { return m_cLeftDepthMapName; }
const string ConfigSyn::getRightDepthMapName() { return m_cRightDepthMapName; }
//const string ConfigSyn::getVirtualViewImageName () { return m_cVirtualViewImageName;  }

const string ConfigSyn::getOutputVirViewImageName() { return m_cOutputVirViewImageName; }

double ConfigSyn::getFocalLength() {
	return m_camParam[0].mIntrinsicMatrix[0][0];
}
double ConfigSyn::getTranslationXLeft() {
	return m_camParam[1].mTranslationVector[0] - m_camParam[0].mTranslationVector[0];
}
double ConfigSyn::getTranslationXRight() {
	return m_camParam[1].mTranslationVector[0] - m_camParam[2].mTranslationVector[0];
}
double ConfigSyn::getPrincipalXLeft() {
	return m_camParam[1].mIntrinsicMatrix[0][2] - m_camParam[0].mIntrinsicMatrix[0][2];
}
double ConfigSyn::getPrincipalXRight() {
	return m_camParam[1].mIntrinsicMatrix[0][2] - m_camParam[2].mIntrinsicMatrix[0][2];
}

double* ConfigSyn::getMat_Rot_Left() {
	return m_camParam[0].getRotationMatrix();
}
double* ConfigSyn::getMat_Rot_Virtual() {
	return m_camParam[1].getRotationMatrix();
}
double* ConfigSyn::getMat_Rot_Right() {
	return m_camParam[2].getRotationMatrix();
}
double* ConfigSyn::getMat_In_Left() {
	return m_camParam[0].getIntrinsicMatrix();
}
double* ConfigSyn::getMat_In_Virtual() {
	return m_camParam[1].getIntrinsicMatrix();
}
double* ConfigSyn::getMat_In_Right() {
	return m_camParam[2].getIntrinsicMatrix();
}
double* ConfigSyn::getMat_Trans_Left() {
	return m_camParam[0].getTranslationVector();
}
double* ConfigSyn::getMat_Trans_Virtual() {
	return m_camParam[1].getTranslationVector();
}
double* ConfigSyn::getMat_Trans_Right() {
	return m_camParam[2].getTranslationVector();
}

double ConfigSyn::getLeftBaselineDistance() { return m_dLeftBaselineDistance; }
double ConfigSyn::getRightBaselineDistance() { return m_dRightBaselineDistance; }

// Access algorithm parameter for 1D mode
int ConfigSyn::getSplattingOption() { return m_iSplattingOption; }
int ConfigSyn::getBoundaryGrowth() { return m_iBoundaryGrowth; }
int ConfigSyn::getMergingOption() { return m_iMergingOption; }
int ConfigSyn::getDepthThreshold() { return m_iDepthThreshold; }
int ConfigSyn::getHoleCountThreshold() { return m_iHoleCountThreshold; }
int ConfigSyn::getTemporalImprovementOption() { return m_iTemporalImprovementOption; } //Zhejiang, May,4
int ConfigSyn::getWarpEnhancementOption() { return m_iWarpEnhancementOption; }
int ConfigSyn::getCleanNoiseOption() { return m_iCleanNoiseOption; }

uint ConfigSyn::getColorSpace() { return m_uiColorSpace; }

uint ConfigSyn::getPrecision() { return m_uiPrecision; }
uint ConfigSyn::getFilter() { return m_uiFilter; }

uint ConfigSyn::getIvsrsInpaint() { return m_uiIvsrsInpaint; }

uint ConfigSyn::getSynthesisMode() { return m_uiSynthesisMode; }
uint ConfigSyn::getBoundaryNoiseRemoval() { return m_uiBoundaryNoiseRemoval; }
uint ConfigSyn::getViewBlending() { return m_uiViewBlending; }

int  ConfigSyn::getDepthBlendDiff() { return m_iDepthBlendDiff; }


ConfigCam* ConfigSyn::getConfigCams() { return m_camParam; }