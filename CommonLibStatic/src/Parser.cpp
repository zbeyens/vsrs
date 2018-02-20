#include "Parser.h"

int Parser::parse()
{
	if (readSynFile() == VSRS_ERROR) return VSRS_ERROR;
	readFromCommandLine();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	cfg.setParams(mParams);
#if 1
	cfg.printParams();
#endif

	if (readCameraFile() == VSRS_ERROR) return VSRS_ERROR;

	if (cfg.validation() == VSRS_ERROR) return VSRS_ERROR;

	return VSRS_OK;
}

int Parser::readSynFile()
{
	//tag - value - empty

	InputStream istream(mFilename.c_str());
	istream.startReading();

	if (istream.hasError()) return VSRS_ERROR;

	while (!istream.isEndOfFile())
	{
		string tagValue[2];
		readLine(istream, tagValue);

		if (tagValue[0].empty() || tagValue[1].empty()) continue;

		mParams[tagValue[0]] = tagValue[1];
	}

	istream.endReading();
	return VSRS_OK;
}

void Parser::readLine(InputStream istream, string* tagValue)
{
	int tagValueIndex = 0;
	bool bComment = false;

	while (1)
	{
		char cchar = istream.readChar();

		if (cchar == EOL || cchar == EOF) return;

		if (cchar == COMMENT)
		{
			bComment = true;
		}
		if (!bComment && tagValueIndex < 2)
		{
			if ((cchar == TAB || cchar == SPACE))
			{
				if (!tagValue[tagValueIndex].empty())
				{
					tagValueIndex++;
				}
			}
			else
			{
				tagValue[tagValueIndex] += cchar;
			}
		}
	}
}

void Parser::readFromCommandLine()
{
	for (int i = 0; i < mCommands.size(); i++)
	{
		char* command = mCommands[i];

		string tagValue[2];
		readCommandLine(command, tagValue);

		if (tagValue[0].empty() || tagValue[1].empty()) {
			cout << "Unknown argument [" << command << "]" << endl;
			continue;
		}

		mParams[tagValue[0]] = tagValue[1];
	}
}

void Parser::readCommandLine(char *buf, string* tagValue)
{
	int tagValueIndex = 0;

	for (int n = 0; buf[n] != EOS && buf[n] != COMMENT; n++)
	{
		if (buf[n] == '=')
		{
			tagValueIndex++;
			continue;
		}
		tagValue[tagValueIndex] += buf[n];
	}
}

int Parser::readCameraFile()
{
	ConfigSyn& cfg = ConfigSyn::getInstance();
	string cameraParameterFile = cfg.getCameraParameterFile();

	InputStream istream(cameraParameterFile.c_str());
	istream.startReading();

	if (istream.hasError()) return VSRS_ERROR;

	const char* cameraId[3];
	cameraId[0] = cfg.getLeftCameraName().c_str();
	cameraId[1] = cfg.getVirtualCameraName().c_str();
	cameraId[2] = cfg.getRightCameraName().c_str();

	for (int i = 0; i < 3; i++)
	{
		if (parseOneCamera(istream, cameraId[i], i) == VSRS_ERROR) return VSRS_ERROR;
	}

	return VSRS_OK;
}

int Parser::parseOneCamera(InputStream istream, const char* cameraId, int cameraIndex)
{
	ConfigSyn& cfg = ConfigSyn::getInstance();
	ConfigCam* configCams = cfg.getConfigCams();

	FILE* fp = istream.getFile();

	istream.seek(0);

	int found = 0;
	char id[255];
	while (parseCameraId(istream, id) == 1)
	{
		if (strcmp(cameraId, id) == 0)
		{
			int read = 0;

			read += parseCameraIntrinsics(istream, cameraIndex);
			read += parseSeparator(istream);
			read += parseCameraExtrinsics(istream, cameraIndex);

			if (read != 23) return VSRS_ERROR;

			found = 1;
			break;
		}
	}

	//configCams[cameraIndex].print();

	if (found == 0)
	{
		cout << "Camera " << cameraId << "is not found in the camera parameter file." << endl;
		return VSRS_ERROR;
	}

	return VSRS_OK;
}

int Parser::parseCameraId(InputStream istream, char * parsedCameraId)
{
	return fscanf(istream.getFile(), "%254s", parsedCameraId);
}

int Parser::parseCameraIntrinsics(InputStream istream, int cameraIndex)
{
	FILE* fp = istream.getFile();

	double intrinsicMatrix[3][3];

	int read = 0;
	read += fscanf(fp, "%lf %lf %lf", &intrinsicMatrix[0][0], &intrinsicMatrix[0][1], &intrinsicMatrix[0][2]);
	read += fscanf(fp, "%lf %lf %lf", &intrinsicMatrix[1][0], &intrinsicMatrix[1][1], &intrinsicMatrix[1][2]);
	read += fscanf(fp, "%lf %lf %lf", &intrinsicMatrix[2][0], &intrinsicMatrix[2][1], &intrinsicMatrix[2][2]);


	ConfigSyn& cfg = ConfigSyn::getInstance();
	ConfigCam* configCams = cfg.getConfigCams();

	configCams[cameraIndex].setIntrinsicMatrix(intrinsicMatrix);

	return read;
}

int Parser::parseSeparator(InputStream istream)
{
	double separator[2];

	int read = 0;
	read += fscanf(istream.getFile(), "%lf %lf", &separator[0], &separator[1]);

	return read;
}

int Parser::parseCameraExtrinsics(InputStream istream, int cameraIndex)
{
	FILE* fp = istream.getFile();

	double extrinsicMatrix[3][3];
	double translationVector[3];

	int read = 0;
	read += fscanf(fp, "%lf %lf %lf %lf", &extrinsicMatrix[0][0], &extrinsicMatrix[0][1], &extrinsicMatrix[0][2], &translationVector[0]);
	read += fscanf(fp, "%lf %lf %lf %lf", &extrinsicMatrix[1][0], &extrinsicMatrix[1][1], &extrinsicMatrix[1][2], &translationVector[1]);
	read += fscanf(fp, "%lf %lf %lf %lf", &extrinsicMatrix[2][0], &extrinsicMatrix[2][1], &extrinsicMatrix[2][2], &translationVector[2]);

	ConfigSyn& cfg = ConfigSyn::getInstance();
	ConfigCam* configCams = cfg.getConfigCams();

	configCams[cameraIndex].setRotationMatrix(extrinsicMatrix);
	configCams[cameraIndex].setTranslationVector(translationVector);

	return read;
}

//Accessors
void Parser::setFilename(string filename)
{
	mFilename = filename;
}

void Parser::setCommands(int argc, char ** argv)
{
	for (int i = 2; i < argc; i++)
	{
		mCommands.push_back(argv[i]);
	}
}