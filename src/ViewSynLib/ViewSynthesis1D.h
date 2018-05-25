/*
 * This software module ViSBD (View Synthesis Based on Disparity /Depth was originally developed by
 * THOMSON INC. in the course of development of the ISO/IEC JTC1/SC29 WG 11 (MPEG) 3D Video for reference
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
 * To the extent that THOMSON INC. OR ANY OF ITS AFFILIATES owns patent rights that would be required to
 * make, use, or sell the originally developed software module or portions thereof included in the ISO/IEC
 * JTC1/SC29 WG 11 (MPEG) 3D Video in a conforming product, THOMSON INC. will assure the ISO/IEC that it
 * is willing to negotiate licenses under reasonable and non-discriminatory terms and conditions with
 * applicants throughout the world.
 *
 * THOMSON INC. retains full right to modify and use the code for its own purpose, assign or donate the
 * code to a third party and to inhibit third parties from using the code for products that do not conform
 * to MPEG-related and/or ISO/IEC International Standards.
 *
 * This copyright notice must be included in all copies or derivative works.
 * Copyright (c) ISO/IEC 2008.
 *
 * Authors:
 *      Dong Tian,  dong.tian@thomson.net
 *      Zefeng Ni,  zefeng.ni@thomson.net
 */

 /*
  * In addition to the orignal authors, this software was further modified by the following parties:
  *
  * Authors:
  *      Lu  Yu,    yul@zju.edu.cn (Zhejiang University)
  *      Yin Zhao,  zhao87099664@163.com  (Zhejiang University)
  *      <new contributor name>, <email>, <affiliation>
  *
  * The related parities retain full right to their code for their own purpose, assign or donate the cooresponding
  * code to another party and to inhibit third parties from using the code for products that do not conform
  * to MPEG-related and/or ISO/IEC International Standards.
  *
  */

#ifndef VISBD_H
#define VISBD_H

#include "ViewSynthesis.h"
#include "ImageResample.h"
#include "BoundaryNoiseRemoval1D.h"

class ViewSynthesis1D : public ViewSynthesis
{
private:
	int m_width;              //!> Picture resolution
	int m_height;
	int m_width2;             //!> The width when the ref view is upsampled (may be 2*Width or 4*Width, it depends). otherwise, it is the same as Width
	int m_subPelOption;       //!> 1: Map the pixel to integer pixels. 2: to half pixels.
	int m_upsampleRefs;       //!> 1: No upsampling refs; 2: Upsampling refs;
							//!> Note that: half pixel precision is obtained by SubPelOption = 2 and UpsampleRefs = 1
							//!>            quar pixel precision is obtained by SubPelOption = 2 and UpsampleRefs = 2
	double LTranslation[2]; //!> The camera distance from the left (0) and right (1) camera
	double duPrincipal[2];  //!> The diff in principal point offset from left (0) and right (1) camera
	double Znear[2];          //!> The real depth value of the nearest  pixel. 0: Left view.  1: Right view
	double Zfar[2];           //!> The real depth value of the farthest pixel. 0: Left view.  1: Right view
	double WeightLeft;      //!> Weighting factor for left  view in blending
	double WeightRight;     //!> Weighting factor for right view in blending

	ImageType* RefView[2];  //!> The upsampled version of the ref view   0 -> Left;   1 -> Right;
	DepthType* RefDepth[2]; //!> The upsampled version of the ref depth  0 -> Left;   1 -> Right;

	//Zhejiang
	int FrameNumber;				          //which frame is being processed
	int WarpToRight;				          //1:warp direction is to the right side; 0: to the left side
	int CountHOLEFILLPIXEL;           //debug using
	int Maxdk, Mindk, Maxz, Minz;     //max and min value in current frame
	int SplattingDepthThreshold;      //determine which area can be regard as background
	ImageType* RefViewLast[2];    //used in TIM: 0->Left; 1->Right
	DepthType* RefDepthLast[2];   //used in TIM: 0->Left; 1->Right

	// Dong Tian: Note that Zmap[3] can be removed once left and right views share the same Znear and Zfar
#if USE_ZMAP
	float        * Zmap[3]; //!> Store the depth values       0 -> Left;   1 -> Right;    2 -> Merged;
#if USE_LDV
	float        * OccZmap[3]; //!> Store the depth values for Occlusion Layer   0 -> Left;   1 -> Right;    2 -> Merged;
#endif
#endif
	unsigned char* Dmap[3]; //!> Store the depth levels       0 -> Left;   1 -> Right;    2 -> Merged;
	unsigned char* Mask[3]; //!> Used for temporary mask  0 -> Left;   1 -> Right;    2 -> Merged;
	unsigned char* BoundaryMask[2]; //!> Used to indicate boudary positions  0 -> Left;   1 -> Right. 255 -> boundary pixel

	unsigned char* SynY[5]; //!> Y. 0 -> Left;   1 -> Right;    2 -> Merged    3 -> After hole filling; 4 -> Downsampled
	unsigned char* SynU[5]; //!> Upsampled UV. 0 -> Left;   1 -> Right;    2 -> Merged;    3 -> After hole filling; 4 -> Downsampled
	unsigned char* SynV[5]; //!> Upsampled UV. 0 -> Left;   1 -> Right;    2 -> Merged;    3 -> After hole filling; 4 -> Downsampled

#if USE_LDV
	unsigned char* ODmap[3]; //!> Store the depth levels for occlusion layer      0 -> Left;   1 -> Right;    2 -> Merged;
	unsigned char* OMask[3]; //!> Used for temporary mask for occlusion layer 0 -> Left;   1 -> Right;    2 -> Merged;
	unsigned char* OccY[5]; //!> Occlusion layer Y. 0 -> Left;   1 -> Right;    2 -> Merged    3 -> After hole filling; 4 -> Downsampled
	unsigned char* OccU[5]; //!> Occlusion layer Upsampled UV. 0 -> Left;   1 -> Right;    2 -> Merged;    3 -> After hole filling; 4 -> Downsampled
	unsigned char* OccV[5]; //!> Occlusion layer Upsampled UV. 0 -> Left;   1 -> Right;    2 -> Merged;    3 -> After hole filling; 4 -> Downsampled
#endif

	Image<ImageType>* m_imgLeftWithHole;		//!> output of the warping for BNR
	Image<ImageType>* m_imgRightWithHole;
	Image<DepthType>* m_depthLeftWithHole;
	Image<DepthType>* m_depthRightWithHole;
	Image<HoleType>* m_holeLeft;
	Image<HoleType>* m_holeRight;

private:
	void PixelMapping(int x, int y, unsigned char d, float z, unsigned char* RefY, unsigned char* RefU, unsigned char* RefV, int ViewId, double dk, int flooring);
	void ForwardWarp();
	void ForwardWarpSingleView(int ViewId);
	void Merge();
	void FillHoles(unsigned char* Yo, unsigned char* Yi);
	int  IsBackgroundOnRight(int xleft, int xright, int y, int blksize);

	void ScaleDownSyn(ImageType* Yo, ImageType* Yi);

	void DetectBoundary(HoleType* edge, DepthType* depth, int width, int height, int threshold);
	bool IsBoundary(unsigned char* BoundaryArray, int x, int y);
	int  CountHolePixels(unsigned char* MaskMap, int x, int y, int width);

	//Zhejiang Univ., new functions
	void FindDepthMaxMin(DepthType* map, int ViewID);
	void TemporalImprovementMethod(ImageType *ImageCur, ImageType*ImageLast, DepthType *DepthCur, DepthType *DepthLast,
		int frame, int Width, int Height);


public:

	ViewSynthesis1D();
	~ViewSynthesis1D();

	void SetFrameNumber(int sFrameNumber) { FrameNumber = sFrameNumber; } //Zhejiang

	int  AllocMem();   //!> Must be called after the resolution is set

	bool  apply(unique_ptr<Image<ImageType>>& outImg);

	void SetLTranslationLeft(double sLTranslationLeft) { LTranslation[LEFTVIEW] = sLTranslationLeft; }
	void SetLTranslationRight(double sLTranslationRight) { LTranslation[RGHTVIEW] = sLTranslationRight; }
	void SetduPrincipalLeft(double sduPrincipalLeft) { duPrincipal[LEFTVIEW] = sduPrincipalLeft; }
	void SetduPrincipalRight(double sduPrincipalRight) { duPrincipal[RGHTVIEW] = sduPrincipalRight; }
	void SetZnearL(double sZnearL) { Znear[LEFTVIEW] = sZnearL; }
	void SetZfarL(double sZfarL) { Zfar[LEFTVIEW] = sZfarL; }
	void SetZnearR(double sZnearR) { Znear[RGHTVIEW] = sZnearR; }
	void SetZfarR(double sZfarR) { Zfar[RGHTVIEW] = sZfarR; }

	void SetPrecision(int sSubPelOption);

	Image<ImageType>*  GetSynLeftWithHole() { return m_imgLeftWithHole; }
	Image<ImageType>*  GetSynRightWithHole() { return m_imgRightWithHole; }
	Image<DepthType>*  GetSynDepthLeftWithHole() { return m_depthLeftWithHole; }
	Image<DepthType>*  GetSynDepthRightWithHole() { return m_depthRightWithHole; }
	Image<HoleType>*  GetSynHoleLeft() { return m_holeLeft; }
	Image<HoleType>*  GetSynHoleRight() { return m_holeRight; }

	int  GetSynMask(unsigned char* SynMask);
	int  GetSynDepth(unsigned char* SynDepth);
	unsigned char* GetSynMaskPointer() { return Mask[MERGVIEW]; }
	unsigned char* GetSynDepthPointer() { return Dmap[MERGVIEW]; }

	unsigned char* GetSynColorLeftY() { return SynY[LEFTVIEW]; }
	unsigned char* GetSynColorRightY() { return SynY[RGHTVIEW]; }
	unsigned char* GetSynColorLeftU() { return SynU[LEFTVIEW]; }
	unsigned char* GetSynColorRightU() { return SynU[RGHTVIEW]; }
	unsigned char* GetSynColorLeftV() { return SynV[LEFTVIEW]; }
	unsigned char* GetSynColorRightV() { return SynV[RGHTVIEW]; }
	unsigned char* GetSynDepthLeft() { return Dmap[LEFTVIEW]; }
	unsigned char* GetSynDepthRight() { return Dmap[RGHTVIEW]; }

	double* GetLTranslation() { return LTranslation; }
	double* GetduPrincipal() { return duPrincipal; }
	double* GetZnear() { return Znear; }
	double* GetZfar() { return Zfar; }
	int     GetSubPelOption() { return m_subPelOption; }
	int     GetUpsampleRefs() { return m_upsampleRefs; }

#if USE_LDV
	unsigned char* GetOccMaskPointer() { return OMask[MERGVIEW]; }
	unsigned char* GetOccDepthPointer() { return ODmap[MERGVIEW]; }
#endif
};

#endif