#include "WarpIpelViewReverse.h"

bool WarpIpelViewReverse::apply(ImageType *** src)
{
	int ptv, u, v;
	int h, w;

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	mView->m_imgVirtualImage.setZero();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 0, 0, w);
			cvmSet(mv, 1, 0, h);
			cvmSet(mv, 2, 0, 1.0);
			cvmSet(mv, 2, 0, 1.0 / mView->m_tableD2Z[mView->m_imgVirtualDepth.getImageY()[h][w]]);
			cvmMul(mView->m_matH_V2R, mv, m);

			u = m->data.db[0] / m->data.db[2] + 0.5;
			v = m->data.db[1] / m->data.db[2] + 0.5;
			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight())
			{
				ptv = w + h * cfg.getSourceWidth();
				mView->m_imgVirtualImage.getImageIpl()->imageData[ptv * 3] = src[0][v][u];
				mView->m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 1] = src[1][v][u];
				mView->m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 2] = src[2][v][u];
			}
		}
	}
	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	if (cfg.getIvsrsInpaint() == 1)
	{
		cvDilate(mView->m_imgHoles.getImageIpl(), mView->m_imgHoles.getImageIpl()); // simple dilate with 3x3 mask
		cvNot(mView->m_imgHoles.getImageIpl(), mView->m_imgSuccessSynthesis.getImageIpl());
	}
	else
	{
		cvCopy(mView->m_imgHoles.getImageIpl(), mView->m_imgBound.getImageIpl());
		erodebound(mView->m_imgBound.getImageIpl(), cfg.getDepthType());
		cvDilate(mView->m_imgBound.getImageIpl(), mView->m_imgBound.getImageIpl());
		cvDilate(mView->m_imgBound.getImageIpl(), mView->m_imgBound.getImageIpl());
		cvOr(mView->m_imgHoles.getImageIpl(), mView->m_imgBound.getImageIpl(), mView->m_imgMask[0].getImageIpl());
	}

	return true;
}
