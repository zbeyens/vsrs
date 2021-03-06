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

#pragma once

#include "SystemIncludes.h"
#include "ViewSynthesis.h"
#include "Image.h"
#include "ImageResample.h"
#include "ConfigSyn.h"
#include "View.h"

#define max2(a,b)        (((a)-(b)>0)?(a):(b))
#define min2(a,b)        (((a)-(b)>0)?(b):(a))
#define guard(value, min, max)   ( max2((min), min2((max), (value)) ) )
#define BOUNDARY_WINDOW_SIZE 7

class BoundaryNoiseRemoval : public ViewSynthesis
{
protected:
	void    xInit();

	virtual void calcWeight() = 0;	//!> Calculating Weighting Factors
	virtual void calcDepthThreshold(bool ViewID) = 0; //!> compute the depthThreshold that is the mean of gap width
	virtual void Blending(shared_ptr<Image<ImageType>> pLeft, shared_ptr<Image<ImageType>> pRight, shared_ptr<Image<ImageType>> outImg) = 0;
	virtual void RemainingHoleFilling(shared_ptr<Image<ImageType>> pSrc) = 0;
	virtual void HoleFillingWithExpandedHole(shared_ptr<Image<ImageType>> pSrc, shared_ptr<Image<ImageType>> pTar, IplImage* m_imgExpandedHole) = 0;

	void copyImages(shared_ptr<Image<ImageType>> pSyn_CurrView, shared_ptr<Image<DepthType>> pSynDepth_CurrView, shared_ptr<Image<HoleType>> pSynHole_CurrView, shared_ptr<Image<HoleType>> pDepthHole_OthView);
	void getBoundaryContour(IplImage* bound, IplImage* contour);
	bool checkFourNeighbours(int i, int j, IplImage* check);
	void getBackgroundContour(IplImage* Bound, IplImage* Depth, IplImage* check_Depth, IplImage* BackBound);
	void expandedHoleforBNM(IplImage* Depth, IplImage* Hole, IplImage* BackBound, IplImage* ExpandedHole);
	void DepthMatchingWithColor(shared_ptr<Image<DepthType>> pDepth, shared_ptr<Image<ImageType>> pColor, shared_ptr<Image<HoleType>> pDepthMask);

	ConfigSyn & cfg;

	int m_precision;	//!> 1 if general mode, cfg.precision if 1D mode

	double m_weightLeft;
	double m_weightRight;


	int m_depthThreshold;
	IplImage* m_imgSynWithHole;
	IplImage* m_imgBound;
	IplImage* m_imgBackBound;
	IplImage* m_imgTemp;
	IplImage* m_imgCheck;
	IplImage* m_imgExpandedHole;
	IplImage* m_imgDepth;
	IplImage* m_imgHoles;
	IplImage* m_imgCommonHole;
	IplImage* m_imgHoleOtherView;

	CvMat* matLeftH_V2R;  // for General Mode
	CvMat* matRightH_V2R;  // for General Mode

	double  m_LeftBaseLineDistance;
	double  m_RightBaseLineDistance;

	double *LTranslation;	//!> The camera distance from the left (0) and right (1) camera
	double *duPrincipal;	//!> The diff in principal point offset from left (0) and right (1) camera
	double *Znear;			//!> The real depth value of the nearest  pixel. 0: Left view.  1: Right view
	double *Zfar;			//!> The real depth value of the farthest pixel. 0: Left view.  1: Right view

	int m_height;		//!> Source height
	int m_height2;		//!>
	int m_width;		//!> Source width
	int m_width2;		//!> The width when the ref view is upsampled (may be 2*Width or 4*Width, it depends). otherwise, it is the same as Width

public:
	BoundaryNoiseRemoval();
	~BoundaryNoiseRemoval();

	bool    apply(shared_ptr<Image<ImageType>> outImg);

	void SetLeftH_V2R(CvMat *sH_V2R) { matLeftH_V2R = sH_V2R; }
	void SetRightH_V2R(CvMat *sH_V2R) { matRightH_V2R = sH_V2R; }

	void SetLTranslationLeft(double *sLTranslation) { LTranslation = sLTranslation; }
	void SetduPrincipal(double *sduPrincipal) { duPrincipal = sduPrincipal; }
	void SetZnear(double *sZnear) { Znear = sZnear; }
	void SetZfar(double *sZfar) { Zfar = sZfar; }

	void SetLeftBaseLineDist(double sDist) { m_LeftBaseLineDistance = sDist; }
	void SetRightBaseLineDist(double sDist) { m_RightBaseLineDistance = sDist; }
	void setViews(vector<shared_ptr<View>> views) { m_views = views; }

};
