
#include <iostream>

using namespace std;


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
	double m_fIntrinsicMatrix[3][3];	//!< Intrinsic parameters
	double m_fExtrinsicMatrix[3][3];	//!< Extrinsic parameters
	double m_fTranslationVector[3];		//!< Translation vector

	/*!
		init:
		-3x3 intrinsic matrix
		-1x3 translation vector
		-3x3 extrinsic matrix
	*/
	ConfigCam();
	~ConfigCam() {};

	//copy
	ConfigCam& operator = (ConfigCam& src);
};