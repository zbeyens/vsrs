#include "Camera.h"

Camera::Camera()
{
}

void Camera::init(uint indexCam)
{
	if (indexCam == 0)
	{
		m_zNear = cfg.getLeftNearestDepthValue();
		m_zFar = cfg.getLeftFarthestDepthValue();
		m_arrayInRef = cfg.getMat_In_Left();
		m_arrayRotRef = cfg.getMat_Rot_Left();
		m_arrayTransRef = cfg.getMat_Trans_Left();
	}
	else if (indexCam == 1)
	{
		m_zNear = cfg.getRightNearestDepthValue();
		m_zFar = cfg.getRightFarthestDepthValue();
		m_arrayInRef = cfg.getMat_In_Right();
		m_arrayRotRef = cfg.getMat_Rot_Right();
		m_arrayTransRef = cfg.getMat_Trans_Right();
	}

	createMats();

	init_camera_param();

	calcSide();
	calcBaselineDistance();

	//release();
}

double Camera::getZnear() { return m_zNear; }
double Camera::getZfar() { return m_zFar; }
double* Camera::getArrayInRef() { return m_arrayInRef; }
double* Camera::getArrayRotRef() { return m_arrayRotRef; }
double* Camera::getArrayTransRef() { return m_arrayTransRef; }

CvMat * Camera::getMatInRef() { return m_matIn[0]; }
CvMat * Camera::getMatInVir() { return m_matIn[1]; }

CvMat * Camera::getMatExRef() { return m_matEx[0]; }
CvMat * Camera::getMatExVir() { return m_matEx[1]; }

CvMat * Camera::getMatProjRef() { return m_matProj[0]; }
CvMat * Camera::getMatProjVir() { return m_matProj[1]; }

bool Camera::isLeftSide() { return m_leftSide; }
double Camera::getBaseline() { return m_baseline; }

void Camera::createMats()
{
	for (int i = 0; i < 2; i++)
	{
		m_matIn[i] = cvCreateMat(3, 3, CV_64F);
		m_matEx[i] = cvCreateMat(3, 4, CV_64F);
		m_matProj[i] = cvCreateMat(3, 4, CV_64F);
	}
}

void Camera::calcSide()
{
	if (cvmGet(getMatExRef(), 0, 3) < cvmGet(getMatExVir(), 0, 3))
		m_leftSide = true;
	else
		m_leftSide = false;
}

void Camera::calcBaselineDistance()
{
	m_baseline = pow((getMatExRef()->data.db[3] - getMatExVir()->data.db[3]), 2.0) +
		pow((getMatExRef()->data.db[7] - getMatExVir()->data.db[7]), 2.0) +
		pow((getMatExRef()->data.db[11] - getMatExVir()->data.db[11]), 2.0);
	m_baseline = sqrt(m_baseline);
}

bool Camera::init_camera_param()
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_matIn[0]->data.db[j + 3 * i] = *(m_arrayInRef + 3 * i + j);
			m_matIn[1]->data.db[j + 3 * i] = *(cfg.getMat_In_Virtual() + 3 * i + j);
		}
	}
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_matEx[0]->data.db[j + 4 * i] = *(m_arrayRotRef + 3 * i + j);
			m_matEx[1]->data.db[j + 4 * i] = *(cfg.getMat_Rot_Virtual() + 3 * i + j);
		}
	}
	for (i = 0; i < 3; i++)
	{
		m_matEx[0]->data.db[3 + 4 * i] = *(m_arrayTransRef + i);
		m_matEx[1]->data.db[3 + 4 * i] = *(cfg.getMat_Trans_Virtual() + i);
	}

	CvMat *MatExDest = cvCreateMat(3, 4, CV_64F);
	for (i = 0; i < 2; i++)
	{
		convertCameraParam(MatExDest, m_matEx[i]);
		cvmMul(m_matIn[i], MatExDest, m_matProj[i]);  // Proj = inMat_c2i * exMat_w2c
		cvmCopy(MatExDest, m_matEx[i]);
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
		cvReleaseMat(&m_matIn[i]);
		cvReleaseMat(&m_matEx[i]);
		cvReleaseMat(&m_matProj[i]);
	}
}