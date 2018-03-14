#include "WarpIpelDepth.h"

bool WarpIpelDepth::apply(DepthType ** pDepthMap)
{
	int h, w, u, v;
	int sigma_d = 20;
	int sigma_c = 50;

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
			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight() && mView->m_imgVirtualDepth.getImageY()[v][u] <= pDepthMap[h][w])
			{
				mView->m_imgVirtualDepth.getImageY()[v][u] = pDepthMap[h][w];
				mView->m_imgSuccessSynthesis.getImageY()[v][u] = MAX_HOLE - 1;
			}
		}
	}

	cvexMedian(mView->m_imgVirtualDepth.getImageIpl());
	cvexBilateral(mView->m_imgVirtualDepth.getImageIpl(), sigma_d, sigma_c);
	cvexMedian(mView->m_imgSuccessSynthesis.getImageIpl());
	cvNot(mView->m_imgSuccessSynthesis.getImageIpl(), mView->m_imgHoles.getImageIpl()); // pixels which couldn't be synthesized
	cvReleaseMat(&m);
	cvReleaseMat(&mv);
	return true;
}

void WarpIpelDepth::cvexMedian(IplImage* dst)
{
	IplImage* buf = cvCloneImage(dst);
	cvSmooth(buf, dst, CV_MEDIAN);
	cvReleaseImage(&buf);
}

void WarpIpelDepth::cvexBilateral(IplImage* dst, int sigma_d, int sigma_c)
{
	IplImage* buf = cvCloneImage(dst);
	cvSmooth(buf, dst, CV_BILATERAL, sigma_d, sigma_c);
	cvReleaseImage(&buf);
}