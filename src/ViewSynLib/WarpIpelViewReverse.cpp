#include "WarpIpelViewReverse.h"

WarpIpelViewReverse::WarpIpelViewReverse()
{
	m_imgBound = new Image<HoleType>();
	m_imgBound->initMat(cfg.getSourceWidth(), cfg.getSourceHeight(), 1);
}

bool WarpIpelViewReverse::apply(View* view)
{
	ImageType*** src = view->getImage()->getData();

	int ptv, u, v;
	int h, w;

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	view->getSynImage()->clearMat();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 0, 0, w);
			cvmSet(mv, 1, 0, h);
			cvmSet(mv, 2, 0, 1.0);
			cvmSet(mv, 2, 0, 1.0 / view->getTableD2Z()[view->getSynDepth()->getY()[h][w]]);
			cvmMul(view->getMatH_V2R(), mv, m);

			u = m->data.db[0] / m->data.db[2] + 0.5;
			v = m->data.db[1] / m->data.db[2] + 0.5;
			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight())
			{
				ptv = w + h * cfg.getSourceWidth();
				view->getSynImage()->getMat()->imageData[ptv * 3] = src[0][v][u];
				view->getSynImage()->getMat()->imageData[ptv * 3 + 1] = src[1][v][u];
				view->getSynImage()->getMat()->imageData[ptv * 3 + 2] = src[2][v][u];
			}
		}
	}
	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEPTH_BASED)
	{
		cvDilate(view->getSynHoles()->getMat(), view->getSynHoles()->getMat()); // simple dilate with 3x3 mask
		cvNot(view->getSynHoles()->getMat(), view->getSynFills()->getMat());
	}
	else
	{
		cvCopy(view->getSynHoles()->getMat(), m_imgBound->getMat());
		erodebound(m_imgBound->getMat(), cfg.getDepthType());
		cvDilate(m_imgBound->getMat(), m_imgBound->getMat());
		cvDilate(m_imgBound->getMat(), m_imgBound->getMat());
		cvOr(view->getSynHoles()->getMat(), m_imgBound->getMat(), view->getImgMask(0)->getMat());
	}

	return true;
}
