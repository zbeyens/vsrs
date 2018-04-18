#include "ConfigSyn.h"

ConfigSyn::ConfigSyn()
	: m_depthType(0)
	, m_sourceWidth(1280)
	, m_sourceHeight(960)
	, m_startFrame(0)
	, m_numberOfFrames(300)
	, m_leftNearestDepthValue(0.0)
	, m_leftFarthestDepthValue(0.0)
	, m_rightNearestDepthValue(0.0)
	, m_rightFarthestDepthValue(0.0)
	, m_cameraParameterFile("cam_param.txt")
	, m_leftCameraName("param_dog38")
	, m_rightCameraName("param_dog39")
	, m_virtualCameraName("param_dog41")
	, m_leftViewImageName("dog038.yuv")
	, m_leftDepthMapName("dog041.yuv")
	, m_rightViewImageName("depth_dog038.yuv")
	, m_rightDepthMapName("depth_dog041.yuv")
	, m_outputVirViewImageName("dog_virtual039.yuv")
	//, m_cVirtualViewImageName		()
	, m_colorSpace(0)
	, m_precision(2)
	, m_filter(1)
	, m_synthesisMode(0)
	, m_boundaryNoiseRemoval(0)
	, m_viewBlending(0)
	, m_depthBlendDiff(5)
	, m_IvsrsInpaint(1)
	, m_splattingOption(2)
	, m_boundaryGrowth(40)
	, m_mergingOption(2)
	, m_depthThreshold(75)
	, m_holeCountThreshold(30)
	, m_temporalImprovementOption(1)
	, m_warpEnhancementOption(1)
	, m_cleanNoiseOption(1)
	, m_testing(0)
	, m_testImageName("../../VSRS_DATA/ResultsDebug/virtual_005003750438_0300_test.yuv")
	//, m_bitDepth(16)
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

		if (tag == "DepthType") m_depthType = atoi(value.c_str());
		else if (tag == "SourceWidth") m_sourceWidth = atoi(value.c_str());
		else if (tag == "SourceHeight") m_sourceHeight = atoi(value.c_str());
		else if (tag == "TotalNumberOfFrames") m_numberOfFrames = atoi(value.c_str());
		else if (tag == "StartFrame") m_startFrame = atoi(value.c_str());

		else if (tag == "LeftNearestDepthValue") m_leftNearestDepthValue = atof(value.c_str());
		else if (tag == "LeftFarthestDepthValue") m_leftFarthestDepthValue = atof(value.c_str());
		else if (tag == "RightNearestDepthValue") m_rightNearestDepthValue = atof(value.c_str());
		else if (tag == "RightFarthestDepthValue") m_rightFarthestDepthValue = atof(value.c_str());

		else if (tag == "CameraParameterFile") m_cameraParameterFile = value;
		else if (tag == "LeftCameraName") m_leftCameraName = value;
		else if (tag == "VirtualCameraName") m_virtualCameraName = value;
		else if (tag == "RightCameraName") m_rightCameraName = value;
		else if (tag == "LeftViewImageName") m_leftViewImageName = value;
		else if (tag == "RightViewImageName") m_rightViewImageName = value;
		else if (tag == "LeftDepthMapName") m_leftDepthMapName = value;
		else if (tag == "RightDepthMapName") m_rightDepthMapName = value;
		//else if (tag == "ReferenceVirtualViewImageName") m_cVirtualViewImageName = value;
		else if (tag == "OutputVirtualViewImageName") m_outputVirViewImageName = value;

		else if (tag == "ColorSpace") m_colorSpace = atoi(value.c_str());
		else if (tag == "Precision") m_precision = atoi(value.c_str());
		else if (tag == "Filter") m_filter = atoi(value.c_str());
		else if (tag == "SynthesisMode") m_synthesisMode = atoi(value.c_str());
		else if (tag == "BoundaryNoiseRemoval") m_boundaryNoiseRemoval = atoi(value.c_str());
		else if (tag == "ViewBlending") m_viewBlending = atoi(value.c_str());
		else if (tag == "DepthBlendDifference") m_depthBlendDiff = atoi(value.c_str());
		else if (tag == "IvsrsInpaint") m_IvsrsInpaint = atoi(value.c_str());
		else if (tag == "SplattingOption") m_splattingOption = atoi(value.c_str());
		else if (tag == "BoundaryGrowth") m_boundaryGrowth = atoi(value.c_str());
		else if (tag == "MergingOption") m_mergingOption = atoi(value.c_str());
		else if (tag == "DepthThreshold") m_depthThreshold = atoi(value.c_str());
		else if (tag == "HoleCountThreshold") m_holeCountThreshold = atoi(value.c_str());
		else if (tag == "TemporalImprovementOption") m_temporalImprovementOption = atoi(value.c_str());
		else if (tag == "WarpEnhancementOption") m_warpEnhancementOption = atoi(value.c_str());
		else if (tag == "CleanNoiseOption") m_cleanNoiseOption = atoi(value.c_str());
		else if (tag == "Testing") m_testing = atoi(value.c_str());
		else if (tag == "TestImageName") m_testImageName = value;
		
		//else if (tag == "BitDepth") m_bitDepth = atoi(value.c_str());
	}
}

void ConfigSyn::printParams()
{
	cout << "DepthType : " << m_depthType << endl;
	cout << "SourceWidth : " << m_sourceWidth << endl;
	cout << "SourceHeight : " << m_sourceHeight << endl;
	cout << "TotalNumberOfFrames : " << m_numberOfFrames << endl;
	cout << "StartFrame : " << m_startFrame << endl;

	cout << "LeftNearestDepthValue : " << m_leftNearestDepthValue << endl;
	cout << "LeftFarthestDepthValue : " << m_leftFarthestDepthValue << endl;
	cout << "RightNearestDepthValue : " << m_rightNearestDepthValue << endl;
	cout << "RightFarthestDepthValue : " << m_rightFarthestDepthValue << endl;

	cout << "CameraParameterFile : " << m_cameraParameterFile << endl;
	cout << "LeftCameraName : " << m_leftCameraName << endl;
	cout << "VirtualCameraName : " << m_virtualCameraName << endl;
	cout << "RightCameraName : " << m_rightCameraName << endl;
	cout << "LeftViewImageName : " << m_leftViewImageName << endl;
	cout << "RightViewImageName : " << m_rightViewImageName << endl;
	cout << "LeftDepthMapName : " << m_leftDepthMapName << endl;
	cout << "RightDepthMapName : " << m_rightDepthMapName << endl;
	//cout << "ReferenceVirtualViewImageName : " << m_cVirtualViewImageName << endl;
	cout << "OutputVirtualViewImageName : " << m_outputVirViewImageName << endl;

	cout << "ColorSpace : " << m_colorSpace << endl;
	cout << "Precision : " << m_precision << endl;
	cout << "Filter : " << m_filter << endl;
	cout << "SynthesisMode : " << m_synthesisMode << endl;
	cout << "BoundaryNoiseRemoval : " << m_boundaryNoiseRemoval << endl;
	cout << "ViewBlending : " << m_viewBlending << endl;
	cout << "DepthBlendDifference : " << m_depthBlendDiff << endl;
	cout << "IvsrsInpaint : " << m_IvsrsInpaint << endl;
	cout << "SplattingOption : " << m_splattingOption << endl;
	cout << "BoundaryGrowth : " << m_boundaryGrowth << endl;
	cout << "MergingOption : " << m_mergingOption << endl;
	cout << "DepthThreshold : " << m_depthThreshold << endl;
	cout << "HoleCountThreshold : " << m_holeCountThreshold << endl;
	cout << "TemporalImprovementOption : " << m_temporalImprovementOption << endl;
	cout << "WarpEnhancementOption : " << m_warpEnhancementOption << endl;
	cout << "CleanNoiseOption : " << m_cleanNoiseOption << endl;
	
	cout << "Testing : " << m_testing << endl;
	cout << "TestImageName : " << m_testImageName << endl;
	//cout << "BitDepth : " << m_bitDepth << endl;
}

uint ConfigSyn::validation()
{
	if (m_precision != 1 && m_precision != 2 && m_precision != 4)
	{
		fprintf(stderr, "Illegal Precision setting\n");
		return VSRS_ERROR;
	}

	if (m_synthesisMode == 1) // 1D mode
	{
		if (m_depthType == 1)
		{
			cout << "Warning: The depth levels must be measured against the cameras. If the world coordinate system does not" << endl;
			cout << "originate from the cameras, synthesis results may be incorrect." << endl;
		}
		if (m_colorSpace != 0)
		{
			cout << "Error: The input format must be in YUV 420 with 1D synthesis mode, for the time being." << endl;
			cout << "       Check ColorSpace." << endl;
			return VSRS_ERROR;
		}

		if (m_boundaryNoiseRemoval && m_splattingOption != 1) {
			if (m_splattingOption != 1) {
				cout << endl << "Warning: When you use 1D Mode, 'Boundary Noise Removal' mode supports only" << endl;
				cout << "         only 'SplattingOption = 1'." << endl;
				cout << "         Hence, 'SplattingOption' is changed to '1'." << endl << endl;
				m_splattingOption = 1;
			}
		}
	}

	// More checks for general mode:
	if (m_synthesisMode == 0) // general mode
	{
	}

	return VSRS_OK;
}

uint ConfigSyn::getDepthType() { return m_depthType; }
int ConfigSyn::getSourceWidth() { return m_sourceWidth; }
int ConfigSyn::getSourceHeight() { return m_sourceHeight; }
uint ConfigSyn::getNumberOfFrames() { return m_numberOfFrames; }
uint ConfigSyn::getStartFrame() { return m_startFrame; }

double ConfigSyn::getLeftNearestDepthValue() { return m_leftNearestDepthValue; }
double ConfigSyn::getLeftFarthestDepthValue() { return m_leftFarthestDepthValue; }
double ConfigSyn::getRightNearestDepthValue() { return m_rightNearestDepthValue; }
double ConfigSyn::getRightFarthestDepthValue() { return m_rightFarthestDepthValue; }

const string ConfigSyn::getCameraParameterFile() { return m_cameraParameterFile; }

const string ConfigSyn::getLeftCameraName() { return m_leftCameraName; }
const string ConfigSyn::getVirtualCameraName() { return m_virtualCameraName; }
const string ConfigSyn::getRightCameraName() { return m_rightCameraName; }

const string ConfigSyn::getLeftViewImageName() { return m_leftViewImageName; }
const string ConfigSyn::getRightViewImageName() { return m_rightViewImageName; }
const string ConfigSyn::getLeftDepthMapName() { return m_leftDepthMapName; }
const string ConfigSyn::getRightDepthMapName() { return m_rightDepthMapName; }

const string ConfigSyn::getOutputVirViewImageName() { return m_outputVirViewImageName; }

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


// Access algorithm parameter for 1D mode
int ConfigSyn::getSplattingOption() { return m_splattingOption; }
int ConfigSyn::getBoundaryGrowth() { return m_boundaryGrowth; }
int ConfigSyn::getMergingOption() { return m_mergingOption; }
int ConfigSyn::getDepthThreshold() { return m_depthThreshold; }
int ConfigSyn::getHoleCountThreshold() { return m_holeCountThreshold; }
int ConfigSyn::getTemporalImprovementOption() { return m_temporalImprovementOption; } //Zhejiang, May,4
int ConfigSyn::getWarpEnhancementOption() { return m_warpEnhancementOption; }
int ConfigSyn::getCleanNoiseOption() { return m_cleanNoiseOption; }

uint ConfigSyn::getColorSpace() { return m_colorSpace; }

uint ConfigSyn::getPrecision() { return m_precision; }
uint ConfigSyn::getFilter() { return m_filter; }

uint ConfigSyn::getIvsrsInpaint() { return m_IvsrsInpaint; }

uint ConfigSyn::getSynthesisMode() { return m_synthesisMode; }
uint ConfigSyn::getBoundaryNoiseRemoval() { return m_boundaryNoiseRemoval; }
uint ConfigSyn::getViewBlending() { return m_viewBlending; }

int  ConfigSyn::getDepthBlendDiff() { return m_depthBlendDiff; }


ConfigCam* ConfigSyn::getConfigCams() { return m_camParam; }