#include "WarpViewReverse.h"

WarpViewReverse::~WarpViewReverse()
{
	if (m_convKernel != NULL)
	{
		cvReleaseStructuringElement(&m_convKernel);
		m_convKernel = NULL;
	}
}

bool WarpViewReverse::init()
{
	int kernel[49] = {
		0,0,1,1,1,0,0,
		0,1,1,1,1,1,0,
		1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,
		0,1,1,1,1,1,0,
		0,0,1,1,1,0,0 };
	m_convKernel = cvCreateStructuringElementEx(7, 7, 3, 3, CV_SHAPE_CUSTOM, kernel);


	return true;
}

bool WarpViewReverse::apply(ImageType *** src, DepthType ** pDepthMap, int th_same_depth)
{
	int ptv, u, v;
	int h, w;
	int maxWidth = cfg.getSourceWidth()*cfg.getPrecision();

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	mView->m_imgVirtualImage.setZero();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			if (mView->m_imgSuccessSynthesis.getImageY()[h][w] == 0) continue;

			ptv = w + h * cfg.getSourceWidth();
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 0, 0, w);
			cvmSet(mv, 1, 0, h);
			cvmSet(mv, 2, 0, 1.0);
			cvmSet(mv, 3, 0, 1.0 / mView->m_dTableD2Z[mView->m_imgVirtualDepth.getImageY()[h][w]]);
			cvmMul(mView->m_matH_V2R, mv, m);

			u = m->data.db[0] * cfg.getPrecision() / m->data.db[2] + 0.5;
			v = m->data.db[1] / m->data.db[2] + 0.5;
			//      if(u>=0 && u<maxWidth && v>=0 && v<height && byte_abs[pDepthMap[v][u/cfg.getPrecision()]-mView->m_imgVirtualDepth.getImageY()[h][w]]<th_same_depth)
			if (u >= 0 && u < maxWidth && v >= 0 && v < cfg.getSourceHeight())
			{
				mView->m_imgVirtualImage.getImageIpl()->imageData[ptv * 3] = src[0][v][u];
				mView->m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 1] = src[1][v][u];
				mView->m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 2] = src[2][v][u];
			}
			else
			{
				mView->m_imgSuccessSynthesis.getImageY()[h][w] = 0;
			}
		}
	}
	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	cvNot(mView->m_imgSuccessSynthesis.getImageIpl(), mView->m_imgHoles.getImageIpl()); // pixels which couldn't be synthesized

	if (cfg.getIvsrsInpaint() == 1)
	{
		cvDilate(mView->m_imgHoles.getImageIpl(), mView->m_imgHoles.getImageIpl()); // simple dilate with 3x3 mask
		cvNot(mView->m_imgHoles.getImageIpl(), mView->m_imgSuccessSynthesis.getImageIpl());
	}
	else
	{
		cvCopy(mView->m_imgHoles.getImageIpl(), mView->m_imgBound.getImageIpl());
		erodebound(mView->m_imgBound.getImageIpl(), mView->cam.isLeftSide());              // background-side boundary of holes
		cvDilate(mView->m_imgBound.getImageIpl(), mView->m_imgBound.getImageIpl(), m_convKernel, 1);  // dilate by using circle-shape kernel
		cvOr(mView->m_imgHoles.getImageIpl(), mView->m_imgBound.getImageIpl(), mView->m_imgMask[0].getImageIpl());    // pixels which want to be modified by other synthesized images
	}

	return true;
}
