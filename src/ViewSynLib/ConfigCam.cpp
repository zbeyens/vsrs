#include "ConfigCam.h"

ConfigCam::ConfigCam()
//: m_fIntrinsicMatrix(3, vector<double>(3))
//, m_fExtrinsicMatrix(3, vector<double>(3))
//, m_fTranslationVector(3)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mIntrinsicMatrix[j][i] = 0.0;
			mRotationMatrix[j][i] = 0.0;
		}
		mTranslationVector[i] = 0.0;
	}
}

ConfigCam::~ConfigCam() {
}

double* ConfigCam::getIntrinsicMatrix()
{
	return &mIntrinsicMatrix[0][0];
}

double* ConfigCam::getRotationMatrix()
{
	return &mRotationMatrix[0][0];
}

double* ConfigCam::getTranslationVector()
{
	return mTranslationVector;
}

void ConfigCam::setIntrinsicMatrix(double intrinsicMatrix[3][3])
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mIntrinsicMatrix[j][i] = intrinsicMatrix[j][i];
		}
	}
}

void ConfigCam::setRotationMatrix(double rotationMatrix[3][3])
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mRotationMatrix[j][i] = rotationMatrix[j][i];
		}
	}
}

void ConfigCam::setTranslationVector(double translationVector[3])
{
	for (int i = 0; i < 3; i++)
	{
		mTranslationVector[i] = translationVector[i];
	}
}

void ConfigCam::print()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << mIntrinsicMatrix[j][i] << " ";
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << mRotationMatrix[j][i] << " ";
		}
		cout << mTranslationVector[i];
		cout << endl;
	}
}