#include "Parser.h"

bool Parser::parse()
{
	if (parseConfigFile() == false) return false;
	parseCommandLine();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	cfg.setParams(m_params);

#if 1
	cfg.printParams();
#endif

	if (parseCameraFile() == false) return false;

	if (cfg.printWarning() == false) return false;

	return true;
}

bool Parser::parseConfigFile()
{
	//tag - value - empty

	InputStream istream(m_filename.c_str());
	istream.openR();

	if (istream.hasError()) return false;

	while (!istream.isEndOfFile())
	{
		string tagValue[2];
		parseLine(istream, tagValue);

		if (tagValue[0].empty() || tagValue[1].empty()) continue;

		m_params[tagValue[0]] = tagValue[1];
	}

	istream.close();
	return true;
}

void Parser::parseLine(InputStream istream, string* tagValue)
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

void Parser::parseCommandLine()
{
	for (int i = 0; i < m_commands.size(); i++)
	{
		char* command = m_commands[i];

		string tagValue[2];
		readCommandLine(command, tagValue);

		if (tagValue[0].empty() || tagValue[1].empty()) {
			cout << "Unknown argument [" << command << "]" << endl;
			continue;
		}

		m_params[tagValue[0]] = tagValue[1];
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

bool Parser::parseCameraFile()
{
	ConfigSyn& cfg = ConfigSyn::getInstance();
	string cameraParameterFile = cfg.getCameraParameterFile();

	InputStream istream(cameraParameterFile);
	istream.openR();

	if (istream.hasError()) return false;

	vector<string> cameraId;

	for (int i = 0; i < cfg.getNViews(); i++)
	{
		cameraId.push_back(cfg.getCameraName(i));
		if (parseOneCamera(istream, cameraId[i].c_str(), i) == false) return false;
	}
	if (parseOneCamera(istream, cfg.getVirtualCameraName().c_str(), -1) == false) return false;

	return true;
}

bool Parser::parseOneCamera(InputStream istream, const char* cameraId, int cameraIndex)
{
	ConfigSyn& cfg = ConfigSyn::getInstance();

	shared_ptr<ConfigCam> configCam;
	if (cameraIndex == -1)
	{
		configCam = cfg.getVirtualConfigCam();
	}
	else
	{
		configCam = cfg.getConfigCams()[cameraIndex];
	}

	FILE* fp = istream.getFile();

	istream.seek(0);

	int found = 0;
	char id[255];
	while (parseCameraId(istream, id) == 1)
	{
		if (strcmp(cameraId, id) == 0)
		{
			int read = 0;

			read += parseCameraIntrinsics(istream, configCam);
			read += parseSeparator(istream);
			read += parseCameraExtrinsics(istream, configCam);

			if (read != 23) return false;

			found = 1;
			break;
		}
	}

	//configCams[cameraIndex].print();

	if (found == 0)
	{
		cout << "Camera " << cameraId << "is not found in the camera parameter file." << endl;
		return false;
	}

	return true;
}

int Parser::parseCameraId(InputStream istream, char * parsedCameraId)
{
	return fscanf(istream.getFile(), "%254s", parsedCameraId);
}

int Parser::parseCameraIntrinsics(InputStream istream, shared_ptr<ConfigCam> configCam)
{
	FILE* fp = istream.getFile();

	double intrinsicMatrix[3][3];

	int read = 0;
	for (size_t i = 0; i < 3; i++)
	{
		read += fscanf(fp, "%lf %lf %lf", &intrinsicMatrix[i][0], &intrinsicMatrix[i][1], &intrinsicMatrix[i][2]);
	}

	configCam->setIntrinsicMatrix(intrinsicMatrix);

	return read;
}

int Parser::parseSeparator(InputStream istream)
{
	double separator[2];

	int read = 0;
	read += fscanf(istream.getFile(), "%lf %lf", &separator[0], &separator[1]);

	return read;
}

int Parser::parseCameraExtrinsics(InputStream istream, shared_ptr<ConfigCam> configCam)
{
	FILE* fp = istream.getFile();

	double extrinsicMatrix[3][3];
	double translationVector[3];

	int read = 0;
	for (size_t i = 0; i < 3; i++)
	{
		read += fscanf(fp, "%lf %lf %lf %lf", &extrinsicMatrix[i][0], &extrinsicMatrix[i][1], &extrinsicMatrix[i][2], &translationVector[i]);
	}

	configCam->setRotationMatrix(extrinsicMatrix);
	configCam->setTranslationVector(translationVector);

	return read;
}

//Accessors
void Parser::setFilename(string filename)
{
	m_filename = filename;
}

void Parser::setCommands(int argc, char ** argv)
{
	for (int i = 2; i < argc; i++)
	{
		m_commands.push_back(argv[i]);
	}
}