/*
* This software module BNR(Boundary Noise Removal) was originally developed by
* Gwangju Institute of Science and Technology (GIST)
* in the course of development of the ISO/IEC JTC1/SC29 WG 11 (MPEG) 3D Video for reference
* purposes and its performance may not have been optimized.
*
* Those intending to use this software module in products are advised that its use may infringe
* existing patents. ISO/IEC have no liability for use of this software module or modifications thereof.
*
* Assurance that the originally developed software module can be used
*   (1) in the ISO/IEC JTC1/SC29 WG 11 (MPEG) 3D Video once the it is adopted to be used as reference
*       software; and
*   (2) to develop the codec for ISO/IEC JTC1/SC29 WG 11 (MPEG) 3D Video.
*
* To the extent that GIST owns patent rights that would be required to
* make, use, or sell the originally developed software module or portions thereof included in the ISO/IEC
* JTC1/SC29 WG 11 (MPEG) 3D Video in a conforming product, GIST will assure the ISO/IEC that it
* is willing to negotiate licenses under reasonable and non-discriminatory terms and conditions with
* applicants throughout the world.
*
* GIST  retains full right to modify and use the code for its own purpose, assign or donate the
* code to a third party and to inhibit third parties from using the code for products that do not conform
* to MPEG-related and/or ISO/IEC International Standards.
*
* This copyright notice must be included in all copies or derivative works.
* Copyright (c) ISO/IEC 2009.
*
* Authors:
*      Cheon Lee,  leecheon@gist.ac.kr
*      Yo-Sung Ho, hoyo@gist.ac.kr
*/

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "BoundaryNoiseRemoval.h"

BoundaryNoiseRemoval::BoundaryNoiseRemoval()
	: cfg(ConfigSyn::getInstance())
{
	m_width = cfg.getSourceWidth();
	m_height = cfg.getSourceHeight();
	m_depthThreshold = 5;

	m_weightLeft = m_weightRight = 0.0;

	LTranslation = NULL;
	duPrincipal = NULL;
	Znear = NULL;
	Zfar = NULL;

	m_imgSynWithHole = NULL;
	m_imgBound = NULL;
	m_imgBackBound = NULL;
	m_imgTemp = NULL;
	m_imgCheck = NULL;
	m_imgExpandedHole = NULL;
	m_imgDepth = NULL;
	m_imgHoles = NULL;
	m_imgCommonHole = NULL;
	m_imgHoleOtherView = NULL;
}

BoundaryNoiseRemoval::~BoundaryNoiseRemoval()
{
	if (m_imgSynWithHole != NULL) { cvReleaseImage(&m_imgSynWithHole); }
	if (m_imgBound != NULL) { cvReleaseImage(&m_imgBound); }
	if (m_imgBackBound != NULL) { cvReleaseImage(&m_imgBackBound); }
	if (m_imgTemp != NULL) { cvReleaseImage(&m_imgTemp); }
	if (m_imgCheck != NULL) { cvReleaseImage(&m_imgCheck); }
	if (m_imgExpandedHole != NULL) { cvReleaseImage(&m_imgExpandedHole); }
	if (m_imgDepth != NULL) { cvReleaseImage(&m_imgDepth); }
	if (m_imgHoles != NULL) { cvReleaseImage(&m_imgHoles); }
	if (m_imgCommonHole != NULL) { cvReleaseImage(&m_imgCommonHole); }
	if (m_imgHoleOtherView != NULL) { cvReleaseImage(&m_imgHoleOtherView); }
}

void BoundaryNoiseRemoval::xInit()
{
	if (m_imgSynWithHole == NULL) { m_imgSynWithHole = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 3); }
	if (m_imgBound == NULL) { m_imgBound = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgBackBound == NULL) { m_imgBackBound = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgTemp == NULL) { m_imgTemp = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgCheck == NULL) { m_imgCheck = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgExpandedHole == NULL) { m_imgExpandedHole = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgDepth == NULL) { m_imgDepth = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgHoles == NULL) { m_imgHoles = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgCommonHole == NULL) { m_imgCommonHole = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
	if (m_imgHoleOtherView == NULL) { m_imgHoleOtherView = cvCreateImage(cvSize(m_width*m_precision, m_height), 8, 1); }
}

bool BoundaryNoiseRemoval::apply(Image<ImageType>* pRefLeft, Image<ImageType>* pRefRight, Image<DepthType>* pRefDepthLeft, Image<DepthType>* pRefDepthRight,
	Image<HoleType>* pRefHoleLeft, Image<HoleType>* pRefHoleRight, unique_ptr<Image<ImageType>>& pSynYuvBuffer, bool SynthesisMode)
{
	int i;
	HoleType *LeftHole, *RightHole;

	xInit();
	LeftHole = pRefHoleLeft->getBuffer1D();
	RightHole = pRefHoleRight->getBuffer1D();
	cvZero(m_imgCommonHole);

	for (i = 0; i < pRefHoleLeft->getWidth() * pRefHoleLeft->getHeight(); i++) {
		if (LeftHole[i] == (MaxTypeValue<HoleType>() - 1) && RightHole[i] == (MaxTypeValue<HoleType>() - 1)) {
			m_imgCommonHole->imageData[i] = (MaxTypeValue<HoleType>() - 1);
		}
	}

	calcWeight();


	// Left
	if (SynthesisMode == cfg.MODE_1D) {
		DepthMatchingWithColor(pRefDepthLeft, pRefLeft, pRefHoleLeft);

		calcDepthThreshold1DMode(0);  // 1D Mode
		copyImages(pRefLeft, pRefDepthLeft, pRefHoleLeft, pRefHoleRight);
		getBoundaryContour(m_imgHoles, m_imgBound);
		getBackgroundContour(m_imgBound, m_imgDepth, m_imgHoles, m_imgBackBound);
		expandedHoleforBNM(m_imgDepth, m_imgHoles, m_imgBackBound, m_imgTemp);
		cvOr(m_imgHoles, m_imgTemp, m_imgExpandedHole);
		HoleFillingWithExpandedHole(pRefRight, pRefLeft, m_imgExpandedHole, SynthesisMode);
		RemainingHoleFilling(pRefLeft);

		DepthMatchingWithColor(pRefDepthRight, pRefRight, pRefHoleRight);
		calcDepthThreshold1DMode(1);  // 1D Mode
		copyImages(pRefRight, pRefDepthRight, pRefHoleRight, pRefHoleLeft);
		getBoundaryContour(m_imgHoles, m_imgBound);
		getBackgroundContour(m_imgBound, m_imgDepth, m_imgHoles, m_imgBackBound);
		expandedHoleforBNM(m_imgDepth, m_imgHoles, m_imgBackBound, m_imgTemp);
		cvOr(m_imgHoles, m_imgTemp, m_imgExpandedHole);
		HoleFillingWithExpandedHole(pRefLeft, pRefRight, m_imgExpandedHole, SynthesisMode);
		RemainingHoleFilling(pRefRight);
	}
	else {
		calcDepthThresholdGeneralMode(matLeftH_V2R);  // General Mode
		copyImages(pRefLeft, pRefDepthLeft, pRefHoleLeft, pRefHoleRight);
		getBoundaryContour(m_imgHoles, m_imgBound);
		getBackgroundContour(m_imgBound, m_imgDepth, m_imgHoles, m_imgBackBound);
		expandedHoleforBNM(m_imgDepth, m_imgHoles, m_imgBackBound, m_imgTemp);
		cvOr(m_imgHoles, m_imgTemp, m_imgExpandedHole);
		HoleFillingWithExpandedHole(pRefRight, pRefLeft, m_imgExpandedHole, SynthesisMode);
		RemainingHoleFilling(pRefLeft);

		calcDepthThresholdGeneralMode(matLeftH_V2R);  // General Mode
		copyImages(pRefRight, pRefDepthRight, pRefHoleRight, pRefHoleLeft);
		getBoundaryContour(m_imgHoles, m_imgBound);
		getBackgroundContour(m_imgBound, m_imgDepth, m_imgHoles, m_imgBackBound);
		expandedHoleforBNM(m_imgDepth, m_imgHoles, m_imgBackBound, m_imgTemp);
		cvOr(m_imgHoles, m_imgTemp, m_imgExpandedHole);
		HoleFillingWithExpandedHole(pRefLeft, pRefRight, m_imgExpandedHole, SynthesisMode);
		RemainingHoleFilling(pRefRight);
	}

	Blending(pRefLeft, pRefRight, pSynYuvBuffer, SynthesisMode);

	return true;
}

void BoundaryNoiseRemoval::calcDepthThreshold1DMode(bool ViewID)
{
	int Low, SumOfGap, index, GapCount, Start_D, End_D, z;
	double posStart, posEnd, GapWidth, dk;

	index = 0;
	Low = 0;
	GapCount = 0;
	SumOfGap = 0;
	posStart = posEnd = 0.0;
	GapWidth = 0.0;
	Start_D = End_D = 0;

	for (index = 1; index < MAX_DEPTH; index++) {
		while (GapWidth < 3) {
			if (++index > (MAX_DEPTH - 1)) {
				break;
			}
			z = 1.0 / ((index / (MaxTypeValue<DepthType>() - 1)) * (1 / Znear[ViewID] - 1 / Zfar[ViewID]) + (1 / Zfar[ViewID]));
			posEnd = (cfg.getFocalLength() * LTranslation[ViewID] / z) - duPrincipal[ViewID];
			GapWidth = fabs(posEnd - posStart);
			End_D = index;
		}
		SumOfGap += abs(End_D - Start_D);
		GapCount++;
		Start_D = End_D;
		posStart = posEnd;
		GapWidth = fabs(posEnd - posStart);
	}
	m_depthThreshold = (int)(SumOfGap / GapCount + 0.5);
}

void BoundaryNoiseRemoval::calcDepthThresholdGeneralMode(CvMat* matH_V2R)
{
	int Low, SumOfGap, index, GapCount, Start_D, End_D;
	float posStart, posEnd, GapWidth;
	index = 0;
	Low = 0;
	GapCount = 0;
	SumOfGap = 0;
	posStart = posEnd = 0.0;
	GapWidth = 0.0;

	CvMat* m = cvCreateMat(4, 1, CV_64F);
	CvMat* mv = cvCreateMat(4, 1, CV_64F);
	cvmSet(mv, 0, 0, 0);
	cvmSet(mv, 1, 0, 0);
	cvmSet(mv, 2, 0, 1);
	cvmSet(mv, 2, 0, 1); //!> ?
	cvmMul(matH_V2R, mv, m);

	posStart = m->data.db[0] * m_precision / m->data.db[2] + 0.5;
	posEnd = m->data.db[0] * m_precision / m->data.db[2] + 0.5;
	GapWidth = fabs(posEnd - posStart);
	Start_D = End_D = 0;

	for (index = 1; index < MAX_DEPTH; index++) {
		while (GapWidth < 3) {
			if (++index > (MAX_DEPTH - 1)) {
				break;
			}
			//#ifdef POZNAN_GENERAL_HOMOGRAPHY
			cvmSet(mv, 3, 0, 1.0 / index);//TableD2Z
			cvmMul(matH_V2R, mv, m);

			posEnd = m->data.db[0] * m_precision / m->data.db[2] + 0.5;
			GapWidth = fabs(posEnd - posStart);
			End_D = index;
		}
		SumOfGap += abs(End_D - Start_D);
		GapCount++;
		Start_D = End_D;
		posStart = posEnd;
		GapWidth = fabs(posEnd - posStart);
	}
	m_depthThreshold = (int)(SumOfGap / GapCount + 0.5);
}

void BoundaryNoiseRemoval::copyImages(Image<ImageType>* pSyn_CurrView, Image<DepthType>* pSynDepth_CurrView, Image<HoleType>* pSynHole_CurrView, Image<HoleType>* pDepthHole_OthView)
{
	int i, j, width, height;
	ImageType *org_buffer_image;
	char *tar_buffer_image;

	DepthType *org_buffer_depth;
	char *tar_buffer_depth;

	HoleType *org_buffer_hole;
	char *tar_buffer_hole;

	cvZero(m_imgSynWithHole);
	cvZero(m_imgDepth);
	cvZero(m_imgHoles);
	cvZero(m_imgHoleOtherView);

	width = m_imgHoles->width;
	height = m_imgHoles->height;

	org_buffer_image = pSyn_CurrView->getBuffer1D();
	tar_buffer_image = m_imgSynWithHole->imageData;
	for (i = 0; i < width*height * 3; i++) {
		tar_buffer_image[i] = org_buffer_image[i];
	}

	org_buffer_depth = pSynDepth_CurrView->getBuffer1D();
	tar_buffer_depth = m_imgDepth->imageData;
	for (i = 0; i < width*height; i++) {
		tar_buffer_depth[i] = org_buffer_depth[i];
	}

	org_buffer_hole = pSynHole_CurrView->getBuffer1D();
	tar_buffer_hole = m_imgHoles->imageData;
	for (i = 0; i < width*height; i++) {
		tar_buffer_hole[i] = org_buffer_hole[i];
	}

	org_buffer_hole = pDepthHole_OthView->getBuffer1D();
	tar_buffer_hole = m_imgHoleOtherView->imageData;
	for (i = 0; i < width*height; i++) {
		tar_buffer_hole[i] = org_buffer_hole[i];
	}
}

void BoundaryNoiseRemoval::getBoundaryContour(IplImage* bound, IplImage* contour)
{
	int i, j, k, width, height, Hole_width, posLeft, posRight;
	width = bound->width;
	height = bound->height;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (bound->imageData[j*width + i] && checkFourNeighbours(i, j, bound)) {
				contour->imageData[j*width + i] = 255;
			}
			else {
				contour->imageData[j*width + i] = 0;
			}
		}
	}
}

bool BoundaryNoiseRemoval::checkFourNeighbours(int i, int j, IplImage* check)
{
	int width = check->width;
	int height = check->height;

	// left (i-1, j)
	if (i - 1 > 0 && check->imageData[j*width + i] && !check->imageData[j*width + i - 1]) {
		return true;
	}

	// right (i+1, j)
	if (i + 1 < width - 1 && check->imageData[j*width + i] && !check->imageData[j*width + i + 1]) {
		return true;
	}

	// up (i, j-1)
	if (j - 1 > 0 && check->imageData[j*width + i] && !check->imageData[(j - 1)*width + i]) {
		return true;
	}

	// down (i, j+1)
	if (j + 1 < height - 1 && check->imageData[j*width + i] && !check->imageData[(j + 1)*width + i]) {
		return true;
	}

	return false;
}

void BoundaryNoiseRemoval::getBackgroundContour(IplImage* Bound, IplImage* Depth, IplImage* check_Depth, IplImage* BackBound)
{
	int i, j, k, width, height;
	int posStart, posEnd, Left_D, Right_D;
	bool bDiff;

	width = Bound->width;
	height = Bound->height;
	cvZero(BackBound);

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (i - 1 < 0 || i + 1 > width - 1) {
				continue;
			}

			if ((uchar)Bound->imageData[j*width + i] == 255) {
				posStart = i;
				Left_D = (uchar)Depth->imageData[j*width + i - 1];
				i = posStart;
				while ((uchar)check_Depth->imageData[j*width + i] && i < width) {
					i++;
				}

				Right_D = (uchar)Depth->imageData[j*width + i + 1];
				posEnd = i;

				abs(Left_D - Right_D) > m_depthThreshold ? bDiff = true : bDiff = false;

				if (abs(posStart - posEnd) <= 3) {
					continue;
				}

				if (bDiff && Left_D < Right_D) {
					BackBound->imageData[j*width + posStart] = 255;
				}
				else if (bDiff && Left_D > Right_D) {
					BackBound->imageData[j*width + posEnd] = 255;
				}
				else {
					for (k = posStart; k < posEnd; k++) {
						Bound->imageData[j*width + k] ? BackBound->imageData[j*width + k] = 255 : BackBound->imageData[j*width + k] = 0;
					}
				}
			}
		}
	}
}

void BoundaryNoiseRemoval::expandedHoleforBNM(IplImage* Depth, IplImage* Hole, IplImage* BackBound, IplImage* ExpandedHole)
{
	int i, j, x, y, width, height, min_x, min_y, max_x, max_y, depth_back;

	width = Depth->width;
	height = Depth->height;
	cvZero(ExpandedHole);

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if ((uchar)BackBound->imageData[j*width + i] == 255) {
				if (i - 1 < 0 || i + 1 > width - 1 || j - 1 < 0 || j + 1 > height - 1) {
					continue;
				}

				min_x = guard(i - BOUNDARY_WINDOW_SIZE, 0, width - 1);
				max_x = guard(i + BOUNDARY_WINDOW_SIZE, 0, width - 1);
				min_y = guard(j - BOUNDARY_WINDOW_SIZE, 0, height - 1);
				max_y = guard(j + BOUNDARY_WINDOW_SIZE, 0, height - 1);

				if ((uchar)Hole->imageData[j*width + i] == 0) {
					depth_back = (uchar)Depth->imageData[j*width + i];
				}
				else if ((uchar)Hole->imageData[j*width + i + 1] == 0) {
					depth_back = (uchar)Depth->imageData[j*width + i + 1];
				}
				else if ((uchar)Hole->imageData[j*width + i - 1] == 0) {
					depth_back = (uchar)Depth->imageData[j*width + i - 1];
				}
				else if ((uchar)Hole->imageData[(j + 1)*width + i] == 0) {
					depth_back = (uchar)Depth->imageData[(j + 1)*width + i];
				}
				else if ((uchar)Hole->imageData[(j - 1)*width + i] == 0) {
					depth_back = (uchar)Depth->imageData[(j - 1)*width + i];
				}
				else {
					continue;
				}

				for (y = min_y; y <= max_y; y++) {
					for (x = min_x; x <= max_x; x++) {
						if (x == 1 && y == 154) {
							int aa = 0;
						}

						if ((uchar)Hole->imageData[y*width + x] == 0
							&& abs((uchar)Depth->imageData[y*width + x] - depth_back) < m_depthThreshold
							&& m_imgHoleOtherView->imageData[y*width + x] == 0) {
							ExpandedHole->imageData[y*width + x] = 255;
						}
						else {
							ExpandedHole->imageData[y*width + x] = 0;
						}
					}
				}
			}
		}
	}
}

void BoundaryNoiseRemoval::DepthMatchingWithColor(Image<DepthType>* pDepth, Image<ImageType>* pColor, Image<HoleType>* pDepthMask)
{
	int i, j, width, height;
	bool isAVailableColor, isAVailableDepth, isAVailableDepth_L, isAVailableDepth_R;
	width = pDepth->getWidth();
	height = pDepth->getHeight();
	ImageType *Y, *U, *V;
	DepthType *D;
	HoleType *D_MASK;
	DepthType D_C, D_L, D_R;

	Y = pColor->getBuffer1D();
	U = &Y[width*height];
	V = &Y[width*height * 2];
	D = pDepth->getBuffer1D();
	D_MASK = pDepthMask->getBuffer1D();

	// Make a hole referring to the color information
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			Y[j*width + i] && U[j*width + i] && V[j*width + i] ? isAVailableColor = true : isAVailableColor = false;
			if (!isAVailableColor) {
				D[j*width + i] = 0;
				D_MASK[j*width + i] = MAX_HOLE - 1;
			}
			else {
				D_MASK[j*width + i] = 0;
			}
		}
	}

	// Fill in a depth hole referring to the adjacent depth information
	for (j = 0; j < height; j++) {
		for (i = 1; i < width - 1; i++) {
			Y[j*width + i] && U[j*width + i] && V[j*width + i] ? isAVailableColor = true : isAVailableColor = false;
			D[j*width + i] ? isAVailableDepth = true : isAVailableDepth = false;
			D[j*width + i - 1] ? isAVailableDepth_L = true : isAVailableDepth_L = false;
			D[j*width + i + 1] ? isAVailableDepth_R = true : isAVailableDepth_R = false;
			D_C = D[j*width + i + 0];
			D_L = D[j*width + i - 1];
			D_R = D[j*width + i + 1];

			if (isAVailableColor && !isAVailableDepth) {
				if (isAVailableDepth_L && isAVailableDepth_R) {
					D[j*width + i] = (BYTE)guard((D_L + D_R + 0.5) / 2, 0, MAX_DEPTH - 1);
					D_MASK[j*width + i] = 0;
				}
				else if (isAVailableDepth_L) {
					D[j*width + i] = D_L;
					D_MASK[j*width + i] = 0;
				}
				else if (isAVailableDepth_R) {
					D[j*width + i] = D_R;
					D_MASK[j*width + i] = 0;
				}
			}
		}
	}
}