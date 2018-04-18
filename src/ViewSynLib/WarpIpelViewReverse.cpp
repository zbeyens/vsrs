#include "WarpIpelViewReverse.h"

bool WarpIpelViewReverse::apply(ImageType *** src)
{
	int ptv, u, v;
	int h, w;

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	m_view->getSynImage()->clearMat();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 0, 0, w);
			cvmSet(mv, 1, 0, h);
			cvmSet(mv, 2, 0, 1.0);
			cvmSet(mv, 2, 0, 1.0 / m_view->m_tableD2Z[m_view->getSynDepth()->getImageY()[h][w]]);
			cvmMul(m_view->m_matH_V2R, mv, m);

			u = m->data.db[0] / m->data.db[2] + 0.5;
			v = m->data.db[1] / m->data.db[2] + 0.5;
			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight())
			{
				ptv = w + h * cfg.getSourceWidth();
				m_view->getSynImage()->getMat()->imageData[ptv * 3] = src[0][v][u];
				m_view->getSynImage()->getMat()->imageData[ptv * 3 + 1] = src[1][v][u];
				m_view->getSynImage()->getMat()->imageData[ptv * 3 + 2] = src[2][v][u];
			}
		}
	}
	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	if (cfg.getIvsrsInpaint() == 1)
	{
		cvDilate(m_view->getSynHoles()->getMat(), m_view->getSynHoles()->getMat()); // simple dilate with 3x3 mask
		cvNot(m_view->getSynHoles()->getMat(), m_view->getSynFills()->getMat());
	}
	else
	{
		cvCopy(m_view->getSynHoles()->getMat(), m_view->m_imgBound.getMat());
		erodebound(m_view->m_imgBound.getMat(), cfg.getDepthType());
		cvDilate(m_view->m_imgBound.getMat(), m_view->m_imgBound.getMat());
		cvDilate(m_view->m_imgBound.getMat(), m_view->m_imgBound.getMat());
		cvOr(m_view->getSynHoles()->getMat(), m_view->m_imgBound.getMat(), m_view->m_imgMask[0].getMat());
	}

	return true;
}
