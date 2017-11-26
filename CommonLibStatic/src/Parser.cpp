#include "Parser.h"


uint Parser::parse()
{
	if (readSynFile() != 1) return -1;
	if (readFromCommandLine() != 1) return -1;

	ConfigSyn& cfg = ConfigSyn::getInstance();

	cfg.setParams(mParams);
#if 1
	cfg.printParams();
#endif

	if (readCameraFile() != 1) return -1;

	//if (cfg.validation() != 1) return -1;

	return 1;
}

int Parser::readSynFile()
{
	//tag - value - empty
	string acTags[3];

	FILE *f = fopen(mFilename.c_str(), "r");
	if (NULL == f)
	{
		cout << "failed to open" << mFilename << "parameter file" << endl;
		return -1;
	}

	while (!feof(f))
	{
		if (readLine(f, acTags) != 1) return -1;

		if (acTags[0].empty())
		{
			continue;
		}

		mParams[acTags[0]] = acTags[1];
	}

	fclose(f);
	return 1;
}



int Parser::readLine(FILE* hFile, string* pacTag)
{
	if (pacTag == NULL) return -1;

	int  n;
	uint uiTagNum = 0;
	bool          bComment = false;
	string*  pcTag = &pacTag[0];

	for (n = 0; n < 3; n++)
	{
		pacTag[n] = "";
	}

	for (n = 0; ; n++)
	{
		char cchar = (char)fgetc(hFile);

		if (cchar == '\n' || feof(hFile)) return 1;
		if (cchar == '#')
		{
			bComment = true;
		}
		if (!bComment)
		{
			if (cchar == '\t' || cchar == ' ') // white space
			{
				if (uiTagNum == 3) return -1;
				//if space after chars
				if (!pcTag->empty())
				{
					uiTagNum++;
					pcTag = &pacTag[uiTagNum];
				}
			}
			else
			{
				*pcTag += cchar;
			}
		}
	}
	return 1;
}

int Parser::readFromCommandLine()
{
	string acTags[2];

	//one param per argv
	for (int i = 0; i < mCommands.size(); i++)
	{
		char* command = mCommands[i];

		if (readCommandLine(command, acTags) != 1)
		{
			cout << "Unknown argument [" << command << "]" << endl;
			continue;
		}
		if (acTags[0].empty())
		{
			continue;
		}

		mParams[acTags[0]] = acTags[1];
	}

	return 1;
}

int Parser::readCommandLine(char *buf, string* pacTag)
{
	if (pacTag == NULL) return -1;

	//0 = tag index, 1 = value index
	uint uiTagNum = 0;
	//pacTag[0] = tag, [1] = value
	string*  pcTag = &pacTag[0];

	int n;
	//reset for the next arg
	for (n = 0; n < 2; n++)
	{
		pacTag[n] = "";
	}

	for (n = 0; buf[n] != '\0' && buf[n] != '#'; n++)
	{
		if (buf[n] == '=')
		{
			//only 1 =
			if (uiTagNum == 1) return -1;
			uiTagNum++;
			pcTag = &pacTag[uiTagNum];
			continue;
		}
		*pcTag += buf[n];
	}

	//unknown or no =
	if (pcTag->empty() || uiTagNum == 0) return -1;

	return 1;
}

uint Parser::readCameraFile()
{
	ConfigSyn& cfg = ConfigSyn::getInstance();

	string cameraParameterFile = cfg.getCameraParameterFile();
	ConfigCam* configCams = cfg.getConfigCams();


	FILE *fp;
	if ((fp = fopen(cameraParameterFile.c_str(), "rt")) == NULL)
	{
		fprintf(stderr, "Can't open camera parameter file: %s\n", cameraParameterFile.c_str());
		return 0;
	}

	const char* cameraId[3];
	cameraId[0] = cfg.getLeftCameraName().c_str();
	cameraId[1] = cfg.getVirtualCameraName().c_str();
	cameraId[2] = cfg.getRightCameraName().c_str();

	for (int i = 0; i < 3; i++)
	{
		if (fseek(fp, 0, SEEK_SET)) return 0;


		int found = 0;
		char id[255];  // To store the cam/view id temporally
		while (fscanf(fp, "%s", id) == 1)
		{
			int read = 0;
			if (strcmp(cameraId[i], id) == 0)
			{
				double gomi[2]; // separator

				read += fscanf(fp, "%lf %lf %lf", &configCams[i].m_fIntrinsicMatrix[0][0], &configCams[i].m_fIntrinsicMatrix[0][1], &configCams[i].m_fIntrinsicMatrix[0][2]);
				read += fscanf(fp, "%lf %lf %lf", &configCams[i].m_fIntrinsicMatrix[1][0], &configCams[i].m_fIntrinsicMatrix[1][1], &configCams[i].m_fIntrinsicMatrix[1][2]);
				read += fscanf(fp, "%lf %lf %lf", &configCams[i].m_fIntrinsicMatrix[2][0], &configCams[i].m_fIntrinsicMatrix[2][1], &configCams[i].m_fIntrinsicMatrix[2][2]);
				read += fscanf(fp, "%lf %lf", &gomi[0], &gomi[1]);
				read += fscanf(fp, "%lf %lf %lf %lf", &configCams[i].m_fExtrinsicMatrix[0][0], &configCams[i].m_fExtrinsicMatrix[0][1], &configCams[i].m_fExtrinsicMatrix[0][2], &configCams[i].m_fTranslationVector[0]);
				read += fscanf(fp, "%lf %lf %lf %lf", &configCams[i].m_fExtrinsicMatrix[1][0], &configCams[i].m_fExtrinsicMatrix[1][1], &configCams[i].m_fExtrinsicMatrix[1][2], &configCams[i].m_fTranslationVector[1]);
				read += fscanf(fp, "%lf %lf %lf %lf", &configCams[i].m_fExtrinsicMatrix[2][0], &configCams[i].m_fExtrinsicMatrix[2][1], &configCams[i].m_fExtrinsicMatrix[2][2], &configCams[i].m_fTranslationVector[2]);
				
				if (read != 23) return 0;

				found = 1;
				break;
			}
		}

		if (found == 0)
		{
			cout << "Camera " << cameraId[i] << "is not found in the camera parameter file." << endl;
			return 0;
		}
	}

	return 1;
}

//Accessors
void Parser::setFileName(string filename)
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
