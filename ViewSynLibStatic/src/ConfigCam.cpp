#include "ConfigCam.h"


ConfigCam::ConfigCam()
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_fIntrinsicMatrix[j][i] = 0.0;
			m_fExtrinsicMatrix[j][i] = 0.0;
		}
		m_fTranslationVector[i] = 0.0;
	}
}

ConfigCam& ConfigCam::operator = (ConfigCam& src)
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_fIntrinsicMatrix[j][i] = src.m_fIntrinsicMatrix[j][i];
			m_fExtrinsicMatrix[j][i] = src.m_fExtrinsicMatrix[j][i];
		}
		m_fTranslationVector[i] = src.m_fTranslationVector[i];
	}

	return (*this);
}