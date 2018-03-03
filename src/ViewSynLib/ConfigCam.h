#pragma once

#include "SystemIncludes.h"

/*!
	\brief
		Store the parameters of one camera

	All camera parameters are estimated with calibration software, used by DERS and VSRS

	For each camera section:
	-the first 3 lines give the camera intrinsics (focal length, principal point,...)
	-the last 3 lines give the camera extrinsics (camera rotation and translation)

	references: CParameterViewInterpolation
*/
class ConfigCam
{
public:
	/*!
	init:
	-3x3 intrinsic matrix
	-1x3 translation vector
	-3x3 rotation matrix
	*/
	ConfigCam();
	~ConfigCam() {}

	//copy
	//ConfigCam(const ConfigCam& src) { operator=(src); }
	ConfigCam& operator = (ConfigCam& src);

	// Print all camera parameters
	void print();

	double* getIntrinsicMatrix();
	double* getRotationMatrix();
	double* getTranslationVector();

	void setIntrinsicMatrix(double intrinsicMatrix[3][3]);
	void setRotationMatrix(double rotationMatrix[3][3]);
	void setTranslationVector(double translationVector[3]);

	double mIntrinsicMatrix[3][3];	//!< Intrinsic parameters
	double mRotationMatrix[3][3];	//!< Rotation matrix
	double mTranslationVector[3];	//!< Translation vector
};