#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"

class Camera
{
public:
	Camera();
	~Camera() {};

	/**
		Get camera values from the config and init
	*/
	void init(uint indexCam);

	double getZnear();
	double getZfar();
	double* getArrayInRef();
	double* getArrayRotRef();
	double* getArrayTransRef();

	CvMat* getMatInRef();
	CvMat* getMatInVir();
	CvMat* getMatExRef();
	CvMat* getMatExVir();
	CvMat* getMatProjRef();
	CvMat* getMatProjVir();

	bool isLeftSide();
	double getBaseline();

private:
	/**
		Fill ex, in and proj 1D matrices
	*/
	bool    init_camera_param();

	
	void createMats();	//!> Create all matrices

	/**
		Compute whether the camera is on the left of the virtual camera (from x-axis).
		All cameras must be placed on the x-axis and their direction be the z-axis (either positive or negative)
		Need to be modify (currently only the special case is supported)
	*/
	void calcSide();
	
	/**
		Compute baseline distance.
		It is the eucl dist between Ref and Vir (using translation column)
	*/
	void calcBaselineDistance();

	/**
		Copy src->dest 3x3 matrices
		transl col = MatEx row * MatEx transl col
	*/
	void convertCameraParam(CvMat *MatExDest, CvMat *MatExSrc);

	void release();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	double m_zNear;
	double m_zFar;
	double* m_arrayInRef;
	double* m_arrayRotRef;
	double* m_arrayTransRef;

	CvMat* m_matIn[2];		//!> Intrinsic parameter of [ref, vir] camera 3x3 matrix
	CvMat* m_matEx[2];		//!> Extrinsic parameter of [ref, vir] camera 3x4 matrix
	CvMat* m_matProj[2];	//!> Projection

	bool  m_leftSide;		//!> if the camera is on the left of the virtual camera

	double m_baseline;		//!> baseline distance of the camera

};