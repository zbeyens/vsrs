#include "WarpDepth.h"

bool WarpDepth::init()
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	m_imgTemp[0].create(sw, sh, 1);
	m_imgTemp[1].create(sw, sh, 1);
	m_imgDepthTemp[0].create(sw, sh, 1);
	m_imgDepthTemp[1].create(sw, sh, 1);

	return true;
}

bool WarpDepth::apply(DepthType **pDepthMap)
{
	int h, w, u, v;
	int window_size = 3;

	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	mView->m_imgVirtualDepth.setZero();
	mView->m_imgSuccessSynthesis.setZero();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			cvmSet(m, 0, 0, w);
			cvmSet(m, 1, 0, h);
			cvmSet(m, 2, 0, 1.0);
			cvmSet(m, 3, 0, 1.0 / mView->m_tableD2Z[pDepthMap[h][w]]);
			cvmMul(mView->m_matH_R2V, m, mv);

			u = mv->data.db[0] / mv->data.db[2] + 0.5;
			v = mv->data.db[1] / mv->data.db[2] + 0.5;

			int iDepth = pDepthMap[h][w];

			double dInvZ = mv->data.db[3] / mv->data.db[2];
			iDepth = (dInvZ - mView->m_invZfar)*(MAX_DEPTH - 1) / mView->m_invZNearMinusInvZFar + 0.5;

			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight() && mView->m_imgVirtualDepth.getImageY()[v][u] <= iDepth)
			{
				mView->m_imgVirtualDepth.getImageY()[v][u] = iDepth;
				mView->m_imgSuccessSynthesis.getImageY()[v][u] = MAX_HOLE - 1;
			}
		}
	}


	cvNot(mView->m_imgSuccessSynthesis.getImageIpl(), mView->m_imgHoles.getImageIpl()); // mView->m_imgHoles.getImageIpl() express holes before smoothing
	cvSmooth(mView->m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(mView->m_imgSuccessSynthesis.getImageIpl(), m_imgTemp[0].getImageIpl(), mView->m_imgMask[0].getImageIpl()); // holes which were not holes before smoothing
	cvCopy(mView->m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), mView->m_imgMask[0].getImageIpl()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getImageIpl(), mView->m_imgSuccessSynthesis.getImageIpl()); // mView->m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getImageIpl(), mView->m_imgHoles.getImageIpl(), CV_MEDIAN, window_size); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(mView->m_imgSuccessSynthesis.getImageIpl(), mView->m_imgHoles.getImageIpl(), mView->m_imgMask[1].getImageIpl()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getImageIpl(), mView->m_imgHoles.getImageIpl(), mView->m_imgMask[1].getImageIpl()); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing

																							  //cvSaveImage("1.bmp", mView->m_imgVirtualDepth);
	cvSmooth(mView->m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), CV_MEDIAN, window_size); // 1st 3x3 median
	cvCopy(mView->m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), mView->m_imgMask[0].getImageIpl());

	cvSmooth(m_imgDepthTemp[1].getImageIpl(), mView->m_imgVirtualDepth.getImageIpl(), CV_MEDIAN, window_size); // 2nd 3x3 median
	cvCopy(m_imgDepthTemp[1].getImageIpl(), mView->m_imgVirtualDepth.getImageIpl(), mView->m_imgMask[1].getImageIpl());

	cvSmooth(mView->m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(mView->m_imgSuccessSynthesis.getImageIpl(), m_imgTemp[0].getImageIpl(), mView->m_imgMask[0].getImageIpl()); // holes which were not holes before smoothing
	cvCopy(mView->m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), mView->m_imgMask[0].getImageIpl()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getImageIpl(), mView->m_imgSuccessSynthesis.getImageIpl()); // mView->m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getImageIpl(), mView->m_imgHoles.getImageIpl(), CV_MEDIAN, window_size); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(mView->m_imgSuccessSynthesis.getImageIpl(), mView->m_imgHoles.getImageIpl(), mView->m_imgMask[1].getImageIpl()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getImageIpl(), mView->m_imgHoles.getImageIpl(), mView->m_imgMask[1].getImageIpl()); // mView->m_imgHoles.getImageIpl() express holes after 2nd smoothing

	cvSmooth(mView->m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), CV_MEDIAN, window_size); // 3rd 3x3 median
	cvCopy(mView->m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), mView->m_imgMask[0].getImageIpl());

	cvSmooth(m_imgDepthTemp[1].getImageIpl(), mView->m_imgVirtualDepth.getImageIpl(), CV_MEDIAN, window_size); // 4th 3x3 median
	cvCopy(m_imgDepthTemp[1].getImageIpl(), mView->m_imgVirtualDepth.getImageIpl(), mView->m_imgMask[1].getImageIpl());

	cvNot(mView->m_imgHoles.getImageIpl(), mView->m_imgSuccessSynthesis.getImageIpl());

	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	return true;
}
