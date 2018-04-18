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

bool WarpViewReverse::apply(ImageType *** src)
{
	int ptv, u, v;
	int h, w;
	int maxWidth = cfg.getSourceWidth()*cfg.getPrecision();

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	m_view->getSynImage()->clearMat();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			if (m_view->getSynFills()->getImageY()[h][w] == 0) continue;

			ptv = w + h * cfg.getSourceWidth();
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 0, 0, w);
			cvmSet(mv, 1, 0, h);
			cvmSet(mv, 2, 0, 1.0);
			cvmSet(mv, 3, 0, 1.0 / m_view->m_tableD2Z[m_view->getSynDepth()->getImageY()[h][w]]);
			cvmMul(m_view->m_matH_V2R, mv, m);

			u = m->data.db[0] * cfg.getPrecision() / m->data.db[2] + 0.5;
			v = m->data.db[1] / m->data.db[2] + 0.5;
			if (u >= 0 && u < maxWidth && v >= 0 && v < cfg.getSourceHeight())
			{
				m_view->getSynImage()->getMat()->imageData[ptv * 3] = src[0][v][u];
				m_view->getSynImage()->getMat()->imageData[ptv * 3 + 1] = src[1][v][u];
				m_view->getSynImage()->getMat()->imageData[ptv * 3 + 2] = src[2][v][u];
			}
			else
			{
				m_view->getSynFills()->getImageY()[h][w] = 0;
			}
		}
	}
	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	cvNot(m_view->getSynFills()->getMat(), m_view->getSynHoles()->getMat()); // pixels which couldn't be synthesized

	if (cfg.getIvsrsInpaint() == 1)
	{
		cvDilate(m_view->getSynHoles()->getMat(), m_view->getSynHoles()->getMat()); // simple dilate with 3x3 mask
		cvNot(m_view->getSynHoles()->getMat(), m_view->getSynFills()->getMat());
	}
	else
	{
		cvCopy(m_view->getSynHoles()->getMat(), m_view->m_imgBound.getMat());
		erodebound(m_view->m_imgBound.getMat(), m_view->getCam().isLeftSide());              // background-side boundary of holes
		cvDilate(m_view->m_imgBound.getMat(), m_view->m_imgBound.getMat(), m_convKernel, 1);  // dilate by using circle-shape kernel
		cvOr(m_view->getSynHoles()->getMat(), m_view->m_imgBound.getMat(), m_view->m_imgMask[0].getMat());    // pixels which want to be modified by other synthesized images
	}

	return true;
}
