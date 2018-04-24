#include "ConfigSyn.h"

ConfigSyn::ConfigSyn()
	: m_depthType(0)
	, m_sourceWidth(1280)
	, m_sourceHeight(960)
	, m_startFrame(0)
	, m_numberOfFrames(300)
	, m_cameraParameterFile("cam_param.txt")
	, m_virtualCameraName("param_dog41")
	, m_outputVirViewImageName("dog_virtual039.yuv")
	, m_nViews(2)
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

void ConfigSyn::initViewsParams()
{
	for (size_t i = 0; i < m_nViews; i++)
	{
		m_nearestDepthValues.push_back(0);
		m_farthestDepthValues.push_back(0);
		m_cameraNames.push_back("");
		m_viewImageNames.push_back("");
		m_depthMapNames.push_back("");
		m_camParams.push_back(new ConfigCam());
	}
	m_virtualConfigCam = new ConfigCam();
}

void ConfigSyn::setParams(map<string, string> params)
{
	for (auto const& param : params)
	{
		string tag = param.first;
		string value = param.second;

		if (tag == "NViews") m_nViews = atoi(value.c_str());
	}

	initViewsParams();

	for (auto const& param : params)
	{
		string tag = param.first;
		string value = param.second;
		//cout << param.first << ':' << param.second << endl;

		for (size_t i = 0; i < m_nViews; i++)
		{
			if (tag == i + "_NearestDepthValue") m_nearestDepthValues[i] = atof(value.c_str());
			else if (tag == i + "_FarthestDepthValue") m_farthestDepthValues[i] = atof(value.c_str());
			else if (tag == i + "_CameraName") m_cameraNames[i] = atof(value.c_str());
			else if (tag == i + "_DepthMapName") m_depthMapNames[i] = atof(value.c_str());
		}

		if (tag == "DepthType") m_depthType = atoi(value.c_str());
		else if (tag == "SourceWidth") m_sourceWidth = atoi(value.c_str());
		else if (tag == "SourceHeight") m_sourceHeight = atoi(value.c_str());
		else if (tag == "TotalNumberOfFrames") m_numberOfFrames = atoi(value.c_str());
		else if (tag == "StartFrame") m_startFrame = atoi(value.c_str());

		
		else if (tag == "LeftNearestDepthValue") m_nearestDepthValues[0] = atof(value.c_str());
		else if (tag == "LeftFarthestDepthValue") m_farthestDepthValues[0] = atof(value.c_str());
		else if (tag == "RightNearestDepthValue") m_nearestDepthValues[1] = atof(value.c_str());
		else if (tag == "RightFarthestDepthValue") m_farthestDepthValues[1] = atof(value.c_str());

		else if (tag == "CameraParameterFile") m_cameraParameterFile = value;
		else if (tag == "VirtualCameraName") m_virtualCameraName = value;
		else if (tag == "LeftCameraName") m_cameraNames[0] = value;
		else if (tag == "RightCameraName") m_cameraNames[1] = value;
		else if (tag == "LeftViewImageName") m_viewImageNames[0] = value;
		else if (tag == "RightViewImageName") m_viewImageNames[1] = value;
		else if (tag == "LeftDepthMapName") m_depthMapNames[0] = value;
		else if (tag == "RightDepthMapName") m_depthMapNames[1] = value;
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

	for (size_t i = 0; i < m_nViews; i++)
	{
		cout << i << "_NearestDepthValue : " << m_nearestDepthValues[i] << endl;
		cout << i << "_FarthestDepthValue : " << m_farthestDepthValues[i] << endl;
		cout << i << "_CameraName : " << m_cameraNames[i] << endl;
		cout << i << "_DepthMapName : " << m_depthMapNames[i] << endl;
	}

	cout << "CameraParameterFile : " << m_cameraParameterFile << endl;
	cout << "VirtualCameraName : " << m_virtualCameraName << endl;
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

const string ConfigSyn::getCameraParameterFile() { return m_cameraParameterFile; }

const string ConfigSyn::getVirtualCameraName() { return m_virtualCameraName; }

const string ConfigSyn::getOutputVirViewImageName() { return m_outputVirViewImageName; }