#include "Sample.h"

int descending(const BYTE *a, const BYTE *b)
{
	if (*a < *b) return 1;
	if (*a > *b) return -1;
	return 0;
}

int ascending(const BYTE *a, const BYTE *b)
{
	if (*a < *b) return -1;
	if (*a > *b) return 1;
	return 0;
}

Sample::Sample()
{
	m_pConvKernel = NULL;
	//Nagoya start
	//Nagoya end
	m_aiTableDisparity_ipel = m_aiTableDisparity_subpel = NULL;


	m_matH_R2V = m_matH_V2R = NULL;
}

Sample::~Sample()
{
	xReleaseMemory();
}

void Sample::xReleaseMemory()
{
	if (m_pConvKernel != NULL)
	{
		cvReleaseStructuringElement(&m_pConvKernel);
		m_pConvKernel = NULL;
	}

	Tools::safeReleaseMat(m_matH_R2V);
	Tools::safeReleaseMat(m_matH_V2R);

	Tools::safeFree(m_aiTableDisparity_ipel);
	Tools::safeFree(m_aiTableDisparity_subpel);
}

bool Sample::Init(uint indexSyn)
{
	mIndexSyn = indexSyn;

	xReleaseMemory();

	if (!initIntermImages()) return false;

	initSynthesisFunctions();
	

	return init_3Dwarp();
}

bool Sample::initIntermImages()
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	//create images
	m_imgVirtualImage.create(sw, sh, IMAGE_CHANNELS);
	m_imgVirtualDepth.create(sw, sh, DEPTH_CHANNELS);
	m_imgSuccessSynthesis.create(sw, sh, 1);
	m_imgHoles.create(sw, sh, 1);
	m_imgBound.create(sw, sh, 1);
	m_imgMask[0].create(sw, sh, MASK_CHANNELS);
	m_imgMask[1].create(sw, sh, MASK_CHANNELS);
	m_imgTemp[0].create(sw, sh, 1);
	m_imgTemp[1].create(sw, sh, 1);
	m_imgDepthTemp[0].create(sw, sh, 1);
	m_imgDepthTemp[1].create(sw, sh, 1);
	

	if (!m_imgVirtualImage.initYUV(m_imgSuccessSynthesis.getImageIpl()->widthStep)) return false;
	if (!m_imgVirtualDepth.initY()) return false;
	if (!m_imgSuccessSynthesis.initY()) return false;


	return true;
}


void Sample::initSynthesisFunctions()
{
	if (cfg.getPrecision() == 1)
	{
		m_pFunc_DepthSynthesis = &Sample::depthsynthesis_3Dwarp_ipel;
		m_pFunc_ViewSynthesisReverse = &Sample::viewsynthesis_reverse_3Dwarp_ipel;
	}
	else
	{
		m_pFunc_DepthSynthesis = &Sample::depthsynthesis_3Dwarp;
		m_pFunc_ViewSynthesisReverse = &Sample::viewsynthesis_reverse_3Dwarp;
	}
}

bool Sample::init_3Dwarp()
{
	int kernel[49] = {
		0,0,1,1,1,0,0,
		0,1,1,1,1,1,0,
		1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,
		0,1,1,1,1,1,0,
		0,0,1,1,1,0,0 };
	m_pConvKernel = cvCreateStructuringElementEx(7, 7, 3, 3, CV_SHAPE_CUSTOM, kernel);

	int i;

	//CameraManager& cameraManager = CameraManager::getInstance();

	cam.init(mIndexSyn);

	double temp1, temp2;

	temp1 = 1.0 / cam.getZnear() - 1.0 / cam.getZfar();
	temp2 = 1.0 / cam.getZfar();

	m_dInvZNearMinusInvZFar = temp1;
	m_dInvZfar = temp2;

	double tableD2Z[MAX_DEPTH];
	double distance;

	switch (cfg.getDepthType())
	{
	case 0:
		for (i = 0; i < MAX_DEPTH; i++)
		{
			distance = 1.0 / (double(i)*temp1 / (MAX_DEPTH - 1.0) + temp2);
			tableD2Z[i] = cvmGet(cam.getMatExRef(), 2, 2) * distance + cam.getArrayTransRef()[2];
			m_dTableD2Z[i] = tableD2Z[i];
		}
		break;
	case 1:
		for (i = 0; i < MAX_DEPTH; i++)
		{
			tableD2Z[i] = 1.0 / (double(i)*temp1 / (MAX_DEPTH - 1.0) + temp2);
			m_dTableD2Z[i] = tableD2Z[i];
		}
		break;
	default:
		return false;
	}

	makeHomography(m_matH_R2V, m_matH_V2R, tableD2Z, cam.getMatInRef(), cam.getMatExRef(), cam.getMatProjVir());

	return true;
}

void Sample::makeHomography(
	CvMat *&matH_F2T, CvMat *&matH_T2F,
	double adTable[MAX_DEPTH], CvMat *MatInFrom, CvMat *MatExFrom, CvMat *MatProjTo)
{
	int i, j, k;
	double val, u, v;
	//CvMat *matIN_inv_from, *mat_Rc2w_invIN_from;
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

	matH_F2T = cvCreateMat(4, 4, CV_64F);
	matH_T2F = cvCreateMat(4, 4, CV_64F);

	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 4; x++)
			cvmSet(matH_F2T, y, x, cvmGet(matH, y, x));
	for (int x = 0; x < 3; x++)
		cvmSet(matH_F2T, 3, x, 0.0);
	cvmSet(matH_F2T, 3, 3, 1.0);

	cvmInvert(matH_F2T, matH_T2F);

	cvReleaseMat(&src_points);
	cvReleaseMat(&dst_points);
	cvReleaseMat(&image);
	cvReleaseMat(&world);
	cvReleaseMat(&matH);
}

void Sample::cvexMedian(IplImage* dst)
{
	IplImage* buf = cvCloneImage(dst);
	cvSmooth(buf, dst, CV_MEDIAN);
	cvReleaseImage(&buf);
}

void Sample::cvexBilateral(IplImage* dst, int sigma_d, int sigma_c)
{
	IplImage* buf = cvCloneImage(dst);
	cvSmooth(buf, dst, CV_BILATERAL, sigma_d, sigma_c);
	cvReleaseImage(&buf);
}

void Sample::erodebound(IplImage* bound, int flag)
{
	int width = bound->width;
	int height = bound->height;
	uchar *ub = (uchar *)bound->imageData;

	if (flag)
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				int l = i + j * width;
				if (ub[l] == 255)
				{
					int ll = l;
					while ((ub[ll] == 255) && (i < width))
					{
						ub[ll] = 0;
						ll++;
						i++;
					}
					ub[ll - 1] = 255;
				}
			}
		}
	}
	else
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				int l = i + j * width;
				if (ub[l] == 255)
				{
					int ll = l;
					while ((ub[ll] == 255) && (i < width))
					{
						ub[ll] = 0;
						ll++;
						i++;
					}
					ub[l] = 255;
				}
			}
		}
	}
}

bool Sample::depthsynthesis_3Dwarp(DepthType **pDepthMap, ImageType ***src)
{
	int h, w, u, v;
	int window_size = 3;

	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	m_imgVirtualDepth.setZero();
	m_imgSuccessSynthesis.setZero();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(m, 0, 0, w);
			cvmSet(m, 1, 0, h);
			cvmSet(m, 2, 0, 1.0);
			cvmSet(m, 3, 0, 1.0 / m_dTableD2Z[pDepthMap[h][w]]);
			cvmMul(m_matH_R2V, m, mv);

			u = mv->data.db[0] / mv->data.db[2] + 0.5;
			v = mv->data.db[1] / mv->data.db[2] + 0.5;

			int iDepth = pDepthMap[h][w];

			//#ifdef POZNAN_DEPTH_PROJECT2COMMON
			double dInvZ = mv->data.db[3] / mv->data.db[2];
			iDepth = (dInvZ - m_dInvZfar)*(MAX_DEPTH - 1) / m_dInvZNearMinusInvZFar + 0.5;

			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight() && m_imgVirtualDepth.getImageY()[v][u] <= iDepth)
			{
				m_imgVirtualDepth.getImageY()[v][u] = iDepth;
				m_imgSuccessSynthesis.getImageY()[v][u] = MAX_HOLE - 1;
			}
		}
	}

	//#if 0 // original median filter
	//	median_filter_depth(m_imgVirtualDepth, m_imgTemp[0], m_imgSuccessSynthesis, m_imgMask[0], 1, 1, true);
	//	median_filter_depth(m_imgTemp[0], m_imgVirtualDepth, m_imgMask[0], m_imgSuccessSynthesis, 1, 1, true);

	cvNot(m_imgSuccessSynthesis.getImageIpl(), m_imgHoles.getImageIpl()); // m_imgHoles.getImageIpl() express holes before smoothing
	cvSmooth(m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(m_imgSuccessSynthesis.getImageIpl(), m_imgTemp[0].getImageIpl(), m_imgMask[0].getImageIpl()); // holes which were not holes before smoothing
	cvCopy(m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), m_imgMask[0].getImageIpl()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getImageIpl(), m_imgSuccessSynthesis.getImageIpl()); // m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getImageIpl(), m_imgHoles.getImageIpl(), CV_MEDIAN, window_size); // m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(m_imgSuccessSynthesis.getImageIpl(), m_imgHoles.getImageIpl(), m_imgMask[1].getImageIpl()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getImageIpl(), m_imgHoles.getImageIpl(), m_imgMask[1].getImageIpl()); // m_imgHoles.getImageIpl() express holes after 2nd smoothing

													//cvSaveImage("1.bmp", m_imgVirtualDepth);
	cvSmooth(m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), CV_MEDIAN, window_size); // 1st 3x3 median
	cvCopy(m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), m_imgMask[0].getImageIpl());

	cvSmooth(m_imgDepthTemp[1].getImageIpl(), m_imgVirtualDepth.getImageIpl(), CV_MEDIAN, window_size); // 2nd 3x3 median
	cvCopy(m_imgDepthTemp[1].getImageIpl(), m_imgVirtualDepth.getImageIpl(), m_imgMask[1].getImageIpl());

	cvSmooth(m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), CV_MEDIAN, window_size); // m_imgTemp[0].getImageIpl() express holes after smoothing
	cvAnd(m_imgSuccessSynthesis.getImageIpl(), m_imgTemp[0].getImageIpl(), m_imgMask[0].getImageIpl()); // holes which were not holes before smoothing
	cvCopy(m_imgHoles.getImageIpl(), m_imgTemp[0].getImageIpl(), m_imgMask[0].getImageIpl()); // m_imgTemp[0].getImageIpl() express holes before 2nd smoothing

	cvNot(m_imgTemp[0].getImageIpl(), m_imgSuccessSynthesis.getImageIpl()); // m_imgSuccessSynthesis express non-holes before 2nd smoothing
	cvSmooth(m_imgTemp[0].getImageIpl(), m_imgHoles.getImageIpl(), CV_MEDIAN, window_size); // m_imgHoles.getImageIpl() express holes after 2nd smoothing
	cvAnd(m_imgSuccessSynthesis.getImageIpl(), m_imgHoles.getImageIpl(), m_imgMask[1].getImageIpl()); // holes which were not holes before 2nd smoothing
	cvCopy(m_imgTemp[0].getImageIpl(), m_imgHoles.getImageIpl(), m_imgMask[1].getImageIpl()); // m_imgHoles.getImageIpl() express holes after 2nd smoothing

	cvSmooth(m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), CV_MEDIAN, window_size); // 3rd 3x3 median
	cvCopy(m_imgVirtualDepth.getImageIpl(), m_imgDepthTemp[1].getImageIpl(), m_imgMask[0].getImageIpl());

	cvSmooth(m_imgDepthTemp[1].getImageIpl(), m_imgVirtualDepth.getImageIpl(), CV_MEDIAN, window_size); // 4th 3x3 median
	cvCopy(m_imgDepthTemp[1].getImageIpl(), m_imgVirtualDepth.getImageIpl(), m_imgMask[1].getImageIpl());

	cvNot(m_imgHoles.getImageIpl(), m_imgSuccessSynthesis.getImageIpl());

	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	return true;
}

bool Sample::viewsynthesis_reverse_3Dwarp(ImageType ***src, DepthType **pDepthMap, int th_same_depth)
{
	int ptv, u, v;
	int h, w;
	int maxWidth = cfg.getSourceWidth()*cfg.getPrecision();

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	m_imgVirtualImage.setZero();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			if (m_imgSuccessSynthesis.getImageY()[h][w] == 0) continue;

			ptv = w + h * cfg.getSourceWidth();
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 0, 0, w);
			cvmSet(mv, 1, 0, h);
			cvmSet(mv, 2, 0, 1.0);
			cvmSet(mv, 3, 0, 1.0 / m_dTableD2Z[m_imgVirtualDepth.getImageY()[h][w]]);
			cvmMul(m_matH_V2R, mv, m);

			u = m->data.db[0] * cfg.getPrecision() / m->data.db[2] + 0.5;
			v = m->data.db[1] / m->data.db[2] + 0.5;
			//      if(u>=0 && u<maxWidth && v>=0 && v<height && byte_abs[pDepthMap[v][u/cfg.getPrecision()]-m_imgVirtualDepth.getImageY()[h][w]]<th_same_depth)
			if (u >= 0 && u < maxWidth && v >= 0 && v < cfg.getSourceHeight())
			{
				m_imgVirtualImage.getImageIpl()->imageData[ptv * 3] = src[0][v][u];
				m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 1] = src[1][v][u];
				m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 2] = src[2][v][u];
			}
			else
			{
				m_imgSuccessSynthesis.getImageY()[h][w] = 0;
			}
		}
	}
	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	cvNot(m_imgSuccessSynthesis.getImageIpl(), m_imgHoles.getImageIpl()); // pixels which couldn't be synthesized

	if (cfg.getIvsrsInpaint() == 1)
	{
		cvDilate(m_imgHoles.getImageIpl(), m_imgHoles.getImageIpl()); // simple dilate with 3x3 mask
		cvNot(m_imgHoles.getImageIpl(), m_imgSuccessSynthesis.getImageIpl());
	}
	else
	{
		cvCopy(m_imgHoles.getImageIpl(), m_imgBound.getImageIpl());
		erodebound(m_imgBound.getImageIpl(), cam.isLeftSide());              // background-side boundary of holes
		cvDilate(m_imgBound.getImageIpl(), m_imgBound.getImageIpl(), m_pConvKernel, 1);  // dilate by using circle-shape kernel
		cvOr(m_imgHoles.getImageIpl(), m_imgBound.getImageIpl(), m_imgMask[0].getImageIpl());    // pixels which want to be modified by other synthesized images
	}

	return true;
}

bool Sample::depthsynthesis_3Dwarp_ipel(DepthType **pDepthMap, ImageType ***src) //Why this is not modyfied like other for new hole filling?? <-- Since it is not useful.
{
	int h, w, u, v;
	int sigma_d = 20;
	int sigma_c = 50;

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	m_imgVirtualDepth.setZero();
	m_imgSuccessSynthesis.setZero();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(m, 0, 0, w);
			cvmSet(m, 1, 0, h);
			cvmSet(m, 2, 0, 1.0);
			cvmSet(m, 3, 0, 1.0 / m_dTableD2Z[pDepthMap[h][w]]);
			cvmMul(m_matH_R2V, m, mv);

			u = mv->data.db[0] / mv->data.db[2] + 0.5;
			v = mv->data.db[1] / mv->data.db[2] + 0.5;
			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight() && m_imgVirtualDepth.getImageY()[v][u] <= pDepthMap[h][w])
			{
				m_imgVirtualDepth.getImageY()[v][u] = pDepthMap[h][w];
				m_imgSuccessSynthesis.getImageY()[v][u] = MAX_HOLE - 1;
			}
		}
	}

	cvexMedian(m_imgVirtualDepth.getImageIpl());
	cvexBilateral(m_imgVirtualDepth.getImageIpl(), sigma_d, sigma_c);
	cvexMedian(m_imgSuccessSynthesis.getImageIpl());
	cvNot(m_imgSuccessSynthesis.getImageIpl(), m_imgHoles.getImageIpl()); // pixels which couldn't be synthesized
	cvReleaseMat(&m);
	cvReleaseMat(&mv);
	return true;
}

bool Sample::viewsynthesis_reverse_3Dwarp_ipel(ImageType ***src, DepthType **pDepthMap, int th_same_depth)
{
	int ptv, u, v;
	int h, w;

	//#ifdef POZNAN_GENERAL_HOMOGRAPHY
	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);

	m_imgVirtualImage.setZero();

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 0, 0, w);
			cvmSet(mv, 1, 0, h);
			cvmSet(mv, 2, 0, 1.0);
			cvmSet(mv, 2, 0, 1.0 / m_dTableD2Z[m_imgVirtualDepth.getImageY()[h][w]]);
			cvmMul(m_matH_V2R, mv, m);

			u = m->data.db[0] / m->data.db[2] + 0.5;
			v = m->data.db[1] / m->data.db[2] + 0.5;
			if (u >= 0 && u < cfg.getSourceWidth() && v >= 0 && v < cfg.getSourceHeight())
			{
				ptv = w + h * cfg.getSourceWidth();
				m_imgVirtualImage.getImageIpl()->imageData[ptv * 3] = src[0][v][u];
				m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 1] = src[1][v][u];
				m_imgVirtualImage.getImageIpl()->imageData[ptv * 3 + 2] = src[2][v][u];
			}
		}
	}
	cvReleaseMat(&m);
	cvReleaseMat(&mv);

	if (cfg.getIvsrsInpaint() == 1)
	{
		cvDilate(m_imgHoles.getImageIpl(), m_imgHoles.getImageIpl()); // simple dilate with 3x3 mask
		cvNot(m_imgHoles.getImageIpl(), m_imgSuccessSynthesis.getImageIpl());
	}
	else
	{
		cvCopy(m_imgHoles.getImageIpl(), m_imgBound.getImageIpl());
		erodebound(m_imgBound.getImageIpl(), cfg.getDepthType());
		cvDilate(m_imgBound.getImageIpl(), m_imgBound.getImageIpl());
		cvDilate(m_imgBound.getImageIpl(), m_imgBound.getImageIpl());
		cvOr(m_imgHoles.getImageIpl(), m_imgBound.getImageIpl(), m_imgMask[0].getImageIpl());
	}

	return true;
}

bool Sample::xSynthesizeView(ImageType ***src, DepthType **pDepthMap, int th_same_depth)
{
	if ((this->*m_pFunc_DepthSynthesis)(pDepthMap, src)) //src
	{
		return (this->*m_pFunc_ViewSynthesisReverse)(src, pDepthMap, th_same_depth);
	}

	return false;
}

bool Sample::xSynthesizeDepth(DepthType **pDepthMap, ImageType ***src)
{
	return (this->*m_pFunc_DepthSynthesis)(pDepthMap, src);
}

bool Sample::xSynthesizeView_reverse(ImageType ***src, DepthType **pDepth, int th_same_depth)
{
	return (this->*m_pFunc_ViewSynthesisReverse)(src, pDepth, th_same_depth);
}

void Sample::image2world_with_z(CvMat *mat_Rc2w_invIN_from, CvMat *MatExFrom, CvMat *image, CvMat *world)
{
	CvMat *temp;
	double s;

	temp = cvCreateMat(3, 1, CV_64F);

	cvmMul(mat_Rc2w_invIN_from, image, temp);

	s = (cvmGet(world, 2, 0) - cvmGet(MatExFrom, 2, 3)) / cvmGet(temp, 2, 0);

	cvmSet(world, 0, 0, s*cvmGet(temp, 0, 0) + cvmGet(MatExFrom, 0, 3));
	cvmSet(world, 1, 0, s*cvmGet(temp, 1, 0) + cvmGet(MatExFrom, 1, 3));

	cvReleaseMat(&temp);
}

int Sample::median_filter_depth(IplImage *srcDepth, IplImage *dstDepth,
	IplImage *srcMask, IplImage *dstMask, int sizeX, int sizeY, bool bSmoothing)
{
	int num;
	int h, w, i, j, ret = 0;
	int size = (2 * sizeX + 1)*(2 * sizeY + 1);
	int th = size / 2;
	BYTE buf[25];
	BYTE *pSrcDepth[5], *pDstDepth[5], *pSrcMask[5], *pDstMask[5];

#ifdef _DEBUG
	if (sizeY > 2 || size > 25) return -1;
#endif

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (i = -sizeY; i <= sizeY; i++)
		{
			if (h + i < 0 || h + i >= cfg.getSourceHeight()) continue;
			pSrcDepth[i + sizeY] = (BYTE *) &(srcDepth->imageData[(h + i)*srcDepth->widthStep]);
			pDstDepth[i + sizeY] = (BYTE *) &(dstDepth->imageData[(h + i)*srcDepth->widthStep]);
			pSrcMask[i + sizeY] = (BYTE *) &(srcMask->imageData[(h + i)*srcDepth->widthStep]);
			pDstMask[i + sizeY] = (BYTE *) &(dstMask->imageData[(h + i)*srcDepth->widthStep]);
		}

		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			pDstDepth[sizeY][w] = pSrcDepth[sizeY][w];
			pDstMask[sizeY][w] = pSrcMask[sizeY][w];

			if (pSrcMask[sizeY][w] != 0 && !bSmoothing) continue;

			num = 0;
			for (i = -sizeY; i <= sizeY; i++)
			{
				if (h + i < 0 || h + i >= cfg.getSourceHeight()) continue;
				for (j = -sizeX; j <= sizeX; j++)
				{
					if (w + j < 0 || w + j >= cfg.getSourceWidth()) continue;
					if (pSrcMask[i + sizeY][w + j] == 0) continue;

					buf[num] = pSrcDepth[i + sizeY][w + j];
					num++;
				}
			}
			if (num > th)
			{
				qsort(buf, num, sizeof(BYTE), (int(*)(const void*, const void*))descending);
				num /= 2;
				pDstDepth[sizeY][w] = buf[num];
				pDstMask[sizeY][w] = MaxTypeValue<HoleType>() - 1;
				ret++;
			}
		}
		}
	return ret;
	}

int Sample::median_filter_depth_wCheck(IplImage *srcDepth, IplImage *dstDepth,
	IplImage *srcMask, IplImage *dstMask,
	int sizeX, int sizeY, bool bSmoothing, int th_same_plane)
{
	int num;
	int h, w, i, j, ret = 0;
	int size = (2 * sizeX + 1)*(2 * sizeY + 1);
	int th = size / 2;
	BYTE buf[25];
	BYTE *pSrcDepth[5], *pDstDepth[5], *pSrcMask[5], *pDstMask[5];

#ifdef _DEBUG
	if (sizeY > 2 || size > 25) return -1;
#endif

	for (h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (i = -sizeY; i <= sizeY; i++)
		{
			if (h + i < 0 || h + i >= cfg.getSourceHeight()) continue;
			pSrcDepth[i + sizeY] = (BYTE *) &(srcDepth->imageData[(h + i)*srcDepth->widthStep]);
			pDstDepth[i + sizeY] = (BYTE *) &(dstDepth->imageData[(h + i)*srcDepth->widthStep]);
			pSrcMask[i + sizeY] = (BYTE *) &(srcMask->imageData[(h + i)*srcDepth->widthStep]);
			pDstMask[i + sizeY] = (BYTE *) &(dstMask->imageData[(h + i)*srcDepth->widthStep]);
		}

		for (w = 0; w < cfg.getSourceWidth(); w++)
		{
			pDstDepth[sizeY][w] = pSrcDepth[sizeY][w];
			pDstMask[sizeY][w] = pSrcMask[sizeY][w];

			if (pSrcMask[sizeY][w] != 0 && !bSmoothing) continue;

			num = 0;
			for (i = -sizeY; i <= sizeY; i++)
			{
				if (h + i < 0 || h + i >= cfg.getSourceHeight()) continue;
				for (j = -sizeX; j <= sizeX; j++)
				{
					if (w + j < 0 || w + j >= cfg.getSourceWidth()) continue;
					if (pSrcMask[i + sizeY][w + j] == 0) continue;

					buf[num] = pSrcDepth[i + sizeY][w + j];
					num++;
				}
			}
			if (num > th)
			{
				qsort(buf, num, sizeof(BYTE), (int(*)(const void*, const void*))descending);
				num /= 2;

				if (abs(int(pDstDepth[sizeY][w]) - int(buf[num])) < th_same_plane)
				{
					pDstDepth[sizeY][w] = buf[num];
					pDstMask[sizeY][w] = MaxTypeValue<HoleType>() - 1;
					ret++;
				}
			}
		}
		}
	return ret;
	}