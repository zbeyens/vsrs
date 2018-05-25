#include "WarpDepth.h"

bool WarpDepth::init()
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	m_imgTemp[0].initMat(sw, sh, 1);
	m_imgTemp[1].initMat(sw, sh, 1);
	m_imgDepthTemp[0].initMat(sw, sh, 1);
	m_imgDepthTemp[1].initMat(sw, sh, 1);

	return true;
}

bool WarpDepth::apply(View* view)
{
	init();

	DepthType** pDepthMap = view->getDepth()->getY();


	int h, w, u, v;
	int window_size = 3;

	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	view->getSynDepth()->clearMat();
	view->getSynFills()->clearMat();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			cvmSet(m, 0, 0, w);
			cvmSet(m, 1, 0, h);
			cvmSet(m, 2, 0, 1.0);
			cvmSet(m, 3, 0, 1.0 / view->getTableD2Z()[pDepthMap[h][w]]);
			cvmMul(view->getMatH_R2V(), m, mv);

			u = mv->data.db[0] / mv->data.db[2] + 0.5;
			v = mv->data.db[1] / mv->data.db[2] + 0.5;

			int iDepth = pDepthMap[h][w];

			double dInvZ = mv->data.db[3] / mv->data.db[2];
			iDepth = (dInvZ - view->getInvZFar())*(MAX_DEPTH - 1) / view->getInvZNearMinusInvZFar() + 0.5;

			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight() && view->getSynDepth()->getY()[v][u] <= iDepth)
			{
				view->getSynDepth()->getY()[v][u] = iDepth;
				view->getSynFills()->getY()[v][u] = MAX_HOLE - 1;
			}
		}
	}

	cvNot(view->getSynFills()->getMat(), view->getSynHoles()->getMat()); // mView->m_imgHoles.getImageIpl() express holes before smoothing
	cvSmooth(view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(view->getSynFills()->getMat(), m_imgTemp[0].getMat(), view->getImgMask(0)->getMat()); // holes which were not holes before smoothing
	cvCopy(view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), view->getImgMask(0)->getMat()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getMat(), view->getSynFills()->getMat()); // mView->m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getMat(), view->getSynHoles()->getMat(), CV_MEDIAN, window_size); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(view->getSynFills()->getMat(), view->getSynHoles()->getMat(), view->getImgMask(1)->getMat()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getMat(), view->getSynHoles()->getMat(), view->getImgMask(1)->getMat()); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing

	cvSmooth(view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), CV_MEDIAN, window_size); // 1st 3x3 median
	cvCopy(view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), view->getImgMask(0)->getMat());

	cvSmooth(m_imgDepthTemp[1].getMat(), view->getSynDepth()->getMat(), CV_MEDIAN, window_size); // 2nd 3x3 median
	cvCopy(m_imgDepthTemp[1].getMat(), view->getSynDepth()->getMat(), view->getImgMask(1)->getMat());

	cvSmooth(view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(view->getSynFills()->getMat(), m_imgTemp[0].getMat(), view->getImgMask(0)->getMat()); // holes which were not holes before smoothing
	cvCopy(view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), view->getImgMask(0)->getMat()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getMat(), view->getSynFills()->getMat()); // mView->m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getMat(), view->getSynHoles()->getMat(), CV_MEDIAN, window_size); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(view->getSynFills()->getMat(), view->getSynHoles()->getMat(), view->getImgMask(1)->getMat()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getMat(), view->getSynHoles()->getMat(), view->getImgMask(1)->getMat()); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing

	cvSmooth(view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), CV_MEDIAN, window_size); // 3rd 3x3 median
	cvCopy(view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), view->getImgMask(0)->getMat());

	cvSmooth(m_imgDepthTemp[1].getMat(), view->getSynDepth()->getMat(), CV_MEDIAN, window_size); // 4th 3x3 median
	cvCopy(m_imgDepthTemp[1].getMat(), view->getSynDepth()->getMat(), view->getImgMask(1)->getMat());

	cvNot(view->getSynHoles()->getMat(), view->getSynFills()->getMat());

	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	return true;
}
