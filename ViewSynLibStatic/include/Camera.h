#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"

class Camera
{
public:
	Camera();
	~Camera() {};

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
	double getBaselineDistance();

private:
	/**
		Fill ex, in and proj 1D matrices
	*/
	bool    init_camera_param();

	//! Create all matrices
	void createMats();

	/**
		Compute whether the camera is on the left of the virtual camera.
		All cameras must be placed on the x-axis and their direction be the z-axis (either positive or negative)
		Need to be modify (currently only the special case is supported)
	*/
	void computeSide();
	
	/**
		Compute baseline distance.
		It is the eucl dist between Ref and Vir (using translation column)
	*/
	void computeBaselineDistance();

	/**
		Copy src->dest 3x3 matrices
		transl col = MatEx row * MatEx transl col
	*/
	void convertCameraParam(CvMat *MatExDest, CvMat *MatExSrc);

	void release();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	double Z_near;
	double Z_far;
	double* mArrayInRef;
	double* mArrayRotRef;
	double* mArrayTransRef;

	CvMat* mMatIn[2];		//!< Intrinsic parameter of [ref, vir] camera 3x3 matrix
	CvMat* mMatEx[2];		//!< Extrinsic parameter of [ref, vir] camera 3x4 matrix
	CvMat* mMatProj[2];		//!< Projection

	bool  mLeftSide;

	double mBaselineDistance;

};