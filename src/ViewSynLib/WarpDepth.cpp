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

bool WarpDepth::apply(DepthType **pDepthMap)
{
	int h, w, u, v;
	int window_size = 3;

	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	m_view->getSynDepth()->clearMat();
	m_view->getSynFills()->clearMat();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			cvmSet(m, 0, 0, w);
			cvmSet(m, 1, 0, h);
			cvmSet(m, 2, 0, 1.0);
			cvmSet(m, 3, 0, 1.0 / m_view->m_tableD2Z[pDepthMap[h][w]]);
			cvmMul(m_view->m_matH_R2V, m, mv);

			u = mv->data.db[0] / mv->data.db[2] + 0.5;
			v = mv->data.db[1] / mv->data.db[2] + 0.5;

			int iDepth = pDepthMap[h][w];

			double dInvZ = mv->data.db[3] / mv->data.db[2];
			iDepth = (dInvZ - m_view->m_invZfar)*(MAX_DEPTH - 1) / m_view->m_invZNearMinusInvZFar + 0.5;

			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight() && m_view->getSynDepth()->getImageY()[v][u] <= iDepth)
			{
				m_view->getSynDepth()->getImageY()[v][u] = iDepth;
				m_view->getSynFills()->getImageY()[v][u] = MAX_HOLE - 1;
			}
		}
	}


	cvNot(m_view->getSynFills()->getMat(), m_view->getSynHoles()->getMat()); // mView->m_imgHoles.getImageIpl() express holes before smoothing
	cvSmooth(m_view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(m_view->getSynFills()->getMat(), m_imgTemp[0].getMat(), m_view->m_imgMask[0].getMat()); // holes which were not holes before smoothing
	cvCopy(m_view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), m_view->m_imgMask[0].getMat()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getMat(), m_view->getSynFills()->getMat()); // mView->m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getMat(), m_view->getSynHoles()->getMat(), CV_MEDIAN, window_size); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(m_view->getSynFills()->getMat(), m_view->getSynHoles()->getMat(), m_view->m_imgMask[1].getMat()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getMat(), m_view->getSynHoles()->getMat(), m_view->m_imgMask[1].getMat()); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing

	cvSmooth(m_view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), CV_MEDIAN, window_size); // 1st 3x3 median
	cvCopy(m_view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), m_view->m_imgMask[0].getMat());

	cvSmooth(m_imgDepthTemp[1].getMat(), m_view->getSynDepth()->getMat(), CV_MEDIAN, window_size); // 2nd 3x3 median
	cvCopy(m_imgDepthTemp[1].getMat(), m_view->getSynDepth()->getMat(), m_view->m_imgMask[1].getMat());

	cvSmooth(m_view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(m_view->getSynFills()->getMat(), m_imgTemp[0].getMat(), m_view->m_imgMask[0].getMat()); // holes which were not holes before smoothing
	cvCopy(m_view->getSynHoles()->getMat(), m_imgTemp[0].getMat(), m_view->m_imgMask[0].getMat()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getMat(), m_view->getSynFills()->getMat()); // mView->m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getMat(), m_view->getSynHoles()->getMat(), CV_MEDIAN, window_size); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(m_view->getSynFills()->getMat(), m_view->getSynHoles()->getMat(), m_view->m_imgMask[1].getMat()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getMat(), m_view->getSynHoles()->getMat(), m_view->m_imgMask[1].getMat()); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing

	cvSmooth(m_view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), CV_MEDIAN, window_size); // 3rd 3x3 median
	cvCopy(m_view->getSynDepth()->getMat(), m_imgDepthTemp[1].getMat(), m_view->m_imgMask[0].getMat());

	cvSmooth(m_imgDepthTemp[1].getMat(), m_view->getSynDepth()->getMat(), CV_MEDIAN, window_size); // 4th 3x3 median
	cvCopy(m_imgDepthTemp[1].getMat(), m_view->getSynDepth()->getMat(), m_view->m_imgMask[1].getMat());

	cvNot(m_view->getSynHoles()->getMat(), m_view->getSynFills()->getMat());

	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	return true;
}
