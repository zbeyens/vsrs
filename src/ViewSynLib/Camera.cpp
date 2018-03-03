#include "Camera.h"

Camera::Camera()
{
}

void Camera::init(uint indexCam)
{
	if (indexCam == 0)
	{
		Z_near = cfg.getLeftNearestDepthValue();
		Z_far = cfg.getLeftFarthestDepthValue();
		mArrayInRef = cfg.getMat_In_Left();
		mArrayRotRef = cfg.getMat_Rot_Left();
		mArrayTransRef = cfg.getMat_Trans_Left();
	}
	else if (indexCam == 1)
	{
		Z_near = cfg.getRightNearestDepthValue();
		Z_far = cfg.getRightFarthestDepthValue();
		mArrayInRef = cfg.getMat_In_Right();
		mArrayRotRef = cfg.getMat_Rot_Right();
		mArrayTransRef = cfg.getMat_Trans_Right();
	}

	createMats();

	init_camera_param();

	computeSide();
	computeBaselineDistance();

	//release();
}

double Camera::getZnear() { return Z_near; }
double Camera::getZfar() { return Z_far; }
double* Camera::getArrayInRef() { return mArrayInRef; }
double* Camera::getArrayRotRef() { return mArrayRotRef; }
double* Camera::getArrayTransRef() { return mArrayTransRef; }

CvMat * Camera::getMatInRef() { return mMatIn[0]; }
CvMat * Camera::getMatInVir() { return mMatIn[1]; }

CvMat * Camera::getMatExRef() { return mMatEx[0]; }
CvMat * Camera::getMatExVir() { return mMatEx[1]; }

CvMat * Camera::getMatProjRef() { return mMatProj[0]; }
CvMat * Camera::getMatProjVir() { return mMatProj[1]; }

bool Camera::isLeftSide() { return mLeftSide; }
double Camera::getBaselineDistance() { return mBaselineDistance; }

void Camera::createMats()
{
	for (int i = 0; i < 2; i++)
	{
		mMatIn[i] = cvCreateMat(3, 3, CV_64F);
		mMatEx[i] = cvCreateMat(3, 4, CV_64F);
		mMatProj[i] = cvCreateMat(3, 4, CV_64F);
	}
}

void Camera::computeSide()
{
	if (cvmGet(getMatExRef(), 0, 3) < cvmGet(getMatExVir(), 0, 3))
		mLeftSide = true;
	else
		mLeftSide = false;
}

void Camera::computeBaselineDistance()
{
	mBaselineDistance = pow((getMatExRef()->data.db[3] - getMatExVir()->data.db[3]), 2.0) +
		pow((getMatExRef()->data.db[7] - getMatExVir()->data.db[7]), 2.0) +
		pow((getMatExRef()->data.db[11] - getMatExVir()->data.db[11]), 2.0);
	mBaselineDistance = sqrt(mBaselineDistance);
}

bool Camera::init_camera_param()
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			mMatIn[0]->data.db[j + 3 * i] = *(mArrayInRef + 3 * i + j);
			mMatIn[1]->data.db[j + 3 * i] = *(cfg.getMat_In_Virtual() + 3 * i + j);
		}
	}
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			mMatEx[0]->data.db[j + 4 * i] = *(mArrayRotRef + 3 * i + j);
			mMatEx[1]->data.db[j + 4 * i] = *(cfg.getMat_Rot_Virtual() + 3 * i + j);
		}
	}
	for (i = 0; i < 3; i++)
	{
		mMatEx[0]->data.db[3 + 4 * i] = *(mArrayTransRef + i);
		mMatEx[1]->data.db[3 + 4 * i] = *(cfg.getMat_Trans_Virtual() + i);
	}

	CvMat *MatExDest = cvCreateMat(3, 4, CV_64F);
	for (i = 0; i < 2; i++)
	{
		convertCameraParam(MatExDest, mMatEx[i]);
		cvmMul(mMatIn[i], MatExDest, mMatProj[i]);  // Proj = inMat_c2i * exMat_w2c
		cvmCopy(MatExDest, mMatEx[i]);
	}
	cvReleaseMat(&MatExDest);

	return true;
}

void Camera::convertCameraParam(CvMat *MatExDest, CvMat *MatExSrc)
{
	int i, j;
	double val;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			cvmSet(MatExDest, i, j, cvmGet(MatExSrc, i, j));
		}
	}

	for (i = 0; i < 3; i++)
	{
		val = 0.0;
		for (j = 0; j < 3; j++)
		{
			val -= cvmGet(MatExDest, i, j)*cvmGet(MatExSrc, j, 3);
		}
		cvmSet(MatExDest, i, 3, val);
	}
}

void Camera::release()
{
	for (int i = 0; i < 2; i++)
	{
		cvReleaseMat(&mMatIn[i]);
		cvReleaseMat(&mMatEx[i]);
		cvReleaseMat(&mMatProj[i]);
	}
}