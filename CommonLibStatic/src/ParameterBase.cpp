//======================================created by Ying Chen =====================================
//===============Tampere University of Technology (TUT)/Nokia Research Center (NRC)===============

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "ParameterBase.h"

#include <string> 

#ifndef MSYS_WIN32
#define stricmp strcasecmp
#endif

#define equal(a,b)  (!stricmp((a),(b)))


using namespace std;


ConfigLineStr::ConfigLineStr(char* pcTag, std::string* pcPar, char* pcDefault) : ConfigLineBase(pcTag, 1), m_pcPar(pcPar)
{
	*m_pcPar = pcDefault;
}

void ConfigLineStr::setVar(std::string& pvValue)
{
	*m_pcPar = pvValue;
}

void ConfigLineStr::fprintVar(FILE *fp)
{
	fprintf(fp, "%s : %s\n", m_cTag.c_str(), m_pcPar->c_str());
}


ConfigLineDbl::ConfigLineDbl(char* pcTag, double* pdPar, double pdDefault) : ConfigLineBase(pcTag, 2), m_pdPar(pdPar)
{
	*m_pdPar = pdDefault;
}

void ConfigLineDbl::setVar(std::string& pvValue)
{
	*m_pdPar = atof(pvValue.c_str());
}

void ConfigLineDbl::fprintVar(FILE *fp)
{
	fprintf(fp, "%s : %f\n", m_cTag.c_str(), *m_pdPar);
}


ConfigLineInt::ConfigLineInt(char* pcTag, int* piPar, int piDefault) : ConfigLineBase(pcTag, 3), m_piPar(piPar)
{
	*m_piPar = piDefault;
}

void ConfigLineInt::setVar(std::string& pvValue)
{
	*m_piPar = atoi(pvValue.c_str());
}

void ConfigLineInt::fprintVar(FILE *fp)
{
	fprintf(fp, "%s : %d\n", m_cTag.c_str(), *m_piPar);
}


ConfigLineuint::ConfigLineuint(char* pcTag, uint* puiPar, uint puiDefault) : ConfigLineBase(pcTag, 4), m_puiPar(puiPar)
{
	*m_puiPar = puiDefault;
}

void ConfigLineuint::setVar(std::string& pvValue)
{
	*m_puiPar = atoi(pvValue.c_str());
}

void ConfigLineuint::fprintVar(FILE *fp)
{
	fprintf(fp, "%s : %d\n", m_cTag.c_str(), *m_puiPar);
}


ConfigLinechar::ConfigLinechar(char* pcTag, char* pcPar, char pcDefault) : ConfigLineBase(pcTag, 5), m_pcPar(pcPar)
{
	*m_pcPar = pcDefault;
}

void ConfigLinechar::setVar(std::string& pvValue)
{
	*m_pcPar = (char)atoi(pvValue.c_str());
}

void ConfigLinechar::fprintVar(FILE *fp)
{
	fprintf(fp, "%s : %c\n", m_cTag.c_str(), *m_pcPar);
}


ParameterBase::ParameterBase()
{
	for (int i = 0; i < MAX_CONFIG_PARAMS; i++) // DT
		m_pCfgLines[i] = NULL;
}

void ParameterBase::release()
{
	uint uiParLnCount = 0;
	while (m_pCfgLines[uiParLnCount] != NULL)
	{
		delete m_pCfgLines[uiParLnCount];
		m_pCfgLines[uiParLnCount] = NULL;
		uiParLnCount++;
	}
}

void ParameterBase::xPrintParam()
{
	uint uiParLnCount = 0;
	while (m_pCfgLines[uiParLnCount] != NULL)
	{
		m_pCfgLines[uiParLnCount]->fprintVar(stdout);
		uiParLnCount++;
	}
}

int ParameterBase::xReadFromFile(std::string& rcFilename)
{
	std::string acTags[4];
	uint        uiParLnCount = 0;
	uint        uiLayerCnt = 0;

	FILE *f = fopen(rcFilename.c_str(), "r");
	if (NULL == f)
	{
		cout << "failed to open" << rcFilename << "parameter file" << endl;
		return -1;
	}

	while (!feof(f))
	{
		if (xReadLine(f, acTags) != 1) return -1;
		if (acTags[0].empty())
		{
			continue;
		}
		for (uint ui = 0; m_pCfgLines[ui] != NULL; ui++)
		{
			//      printf("%s %s \n", acTags[0].c_str(), m_pCfgLines[ui]->getTag().c_str());
			if (acTags[0] == m_pCfgLines[ui]->getTag())
			{
				m_pCfgLines[ui]->setVar(acTags[1]);
				break;
			}
		}
	}

	fclose(f);
	return 1;
}



int ParameterBase::xReadLine(FILE* hFile, std::string* pacTag)
{
	if (pacTag == NULL) return -1;

	int  n;
	uint uiTagNum = 0;
	bool          bComment = false;
	std::string*  pcTag = &pacTag[0];

	for (n = 0; n < 4; n++)
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

int ParameterBase::xReadFromCommandLine(int argc, char **argv)
{
	std::string acTags[2];

	//one param per argv
	for (int i = 2; i < argc; i++)
	{
		if (xReadCommandLine(argv[i], acTags) != 1)
		{
			cout << "Unknown argument [" << argv[i] << "]" << endl;
			continue;
		}
		if (acTags[0].empty())
		{
			continue;
		}

		uint ui;
		for (ui = 0; m_pCfgLines[ui] != NULL; ui++)
		{
			//if tag =, set value
			if (acTags[0] == m_pCfgLines[ui]->getTag())
			{
				m_pCfgLines[ui]->setVar(acTags[1]);
				break;
			}
		}
		if (m_pCfgLines[ui] == NULL)
		{
			cout << "Unknown parameter [" << acTags[0] << "]" << endl;
		}
	}

	return 1;
}

int ParameterBase::xReadCommandLine(char *buf, std::string* pacTag)
{
	if (pacTag == NULL) return -1;

	//0 = tag index, 1 = value index
	uint uiTagNum = 0;
	//pacTag[0] = tag, [1] = value
	std::string*  pcTag = &pacTag[0];

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
