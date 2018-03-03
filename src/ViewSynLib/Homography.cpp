#include "Homography.h"

bool Homography::apply(CvMat *&matH_F2TOut, CvMat *&matH_T2FOut, const CvMat *MatInFrom, const CvMat *MatExFrom, const CvMat *MatProjTo)
{
	CvMat *MatInvProjFrom;
	CvMat *matProj_from;
	CvMat *matProjX_from;
	CvMat *matH;

	CvMat *src_points = cvCreateMat(4, 2, CV_64F);
	CvMat *dst_points = cvCreateMat(4, 2, CV_64F);
	CvMat *image = cvCreateMat(3, 1, CV_64F);
	CvMat *world = cvCreateMat(4, 1, CV_64F);

	matProj_from = cvCreateMat(3, 4, CV_64F);
	cvmMul(MatInFrom, MatExFrom, matProj_from);  // Proj = inMat_c2i * exMat_w2c
	matProjX_from = cvCreateMat(4, 4, CV_64F);
	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 4; x++)
			cvmSet(matProjX_from, y, x, cvmGet(matProj_from, y, x));
	for (int x = 0; x < 3; x++)
		cvmSet(matProjX_from, 3, x, 0.0);
	cvmSet(matProjX_from, 3, 3, 1.0);

	MatInvProjFrom = cvCreateMat(4, 4, CV_64F);
	cvmInvert(matProjX_from, MatInvProjFrom);

	matH = cvCreateMat(3, 4, CV_64F);
	cvmMul(MatProjTo, MatInvProjFrom, matH);

	cvReleaseMat(&MatInvProjFrom);
	cvReleaseMat(&matProj_from);
	cvReleaseMat(&matProjX_from);

	matH_F2TOut = cvCreateMat(4, 4, CV_64F);
	matH_T2FOut = cvCreateMat(4, 4, CV_64F);

	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 4; x++)
			cvmSet(matH_F2TOut, y, x, cvmGet(matH, y, x));
	for (int x = 0; x < 3; x++)
		cvmSet(matH_F2TOut, 3, x, 0.0);
	cvmSet(matH_F2TOut, 3, 3, 1.0);

	cvmInvert(matH_F2TOut, matH_T2FOut);

	cvReleaseMat(&src_points);
	cvReleaseMat(&dst_points);
	cvReleaseMat(&image);
	cvReleaseMat(&world);
	cvReleaseMat(&matH);

	return true;
}
