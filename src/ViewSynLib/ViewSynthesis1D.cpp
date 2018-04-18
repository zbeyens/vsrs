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

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "ViewSynthesis1D.h"

ViewSynthesis1D::ViewSynthesis1D()
{
	m_imagesWidth = m_width;

	int i;

	for (i = 0; i < 2; i++)
	{
		BoundaryMask[i] = NULL;
		RefView[i] = NULL;
		RefDepth[i] = NULL;
		RefViewLast[i] = NULL; //Zhejiang
		RefDepthLast[i] = NULL;
	}
	for (i = 0; i < 3; i++)
	{
		Mask[i] = NULL;
		Dmap[i] = NULL;
#if USE_LDV
		OMask[i] = NULL;
		ODmap[i] = NULL;
#endif
#if USE_ZMAP
		Zmap[i] = NULL;
#if USE_LDV
		OccZmap[i] = NULL;
#endif
#endif
	}
	for (i = 0; i < 5; i++)
	{
		SynY[i] = NULL;
		SynU[i] = NULL;
		SynV[i] = NULL;
#if USE_LDV
		OccY[i] = NULL;
		OccU[i] = NULL;
		OccV[i] = NULL;
#endif
	}

	m_upsampleRefs = 2;
	SplattingDepthThreshold = 255;

	SetLTranslationLeft(cfg.getTranslationXLeft());
	SetLTranslationRight(cfg.getTranslationXRight());
	SetduPrincipalLeft(cfg.getPrincipalXLeft());
	SetduPrincipalRight(cfg.getPrincipalXRight());
	SetZnearL(cfg.getLeftNearestDepthValue());
	SetZfarL(cfg.getLeftFarthestDepthValue());
	SetZnearR(cfg.getRightNearestDepthValue());
	SetZfarR(cfg.getRightFarthestDepthValue());
	SetPrecision(cfg.getPrecision());

	m_width = cfg.getSourceWidth();
	m_height = cfg.getSourceHeight();

	m_imgLeftWithHole = new Image<ImageType>(m_height, m_width2, IMAGE_CHROMA_FORMAT);
	m_imgRightWithHole = new Image<ImageType>(m_height, m_width2, IMAGE_CHROMA_FORMAT);
	m_depthLeftWithHole = new Image<DepthType>(m_height, m_width2, DEPTHMAP_CHROMA_FORMAT);
	m_depthRightWithHole = new Image<DepthType>(m_height, m_width2, DEPTHMAP_CHROMA_FORMAT);
	m_holeLeft = new Image<HoleType>(m_height, m_width2, HOLE_CHROMA_FORMAT);
	m_holeRight = new Image<HoleType>(m_height, m_width2, HOLE_CHROMA_FORMAT);

	AllocMem();
}

ViewSynthesis1D::~ViewSynthesis1D()
{
	Tools::safeDelete(m_imgLeftWithHole);
	Tools::safeDelete(m_imgRightWithHole);
	Tools::safeDelete(m_depthLeftWithHole);
	Tools::safeDelete(m_depthRightWithHole);
	Tools::safeDelete(m_holeLeft);
	Tools::safeDelete(m_holeRight);

	int i;
	for (i = 0; i < 2; i++)
	{
		if (BoundaryMask[i]) free(BoundaryMask[i]);
		if (m_upsampleRefs != 1)
		{
			if (RefView[i]) free(RefView[i]);
			if (RefDepth[i]) free(RefDepth[i]);
			if (RefViewLast[i]) free(RefViewLast[i]); //Zhejiang
			if (RefDepthLast[i]) free(RefDepthLast[i]);
		}
	}
	for (i = 0; i < 3; i++)
	{
		if (Mask[i]) free(Mask[i]);
		if (Dmap[i]) free(Dmap[i]);
#if USE_LDV
		if (OMask[i]) free(OMask[i]);
		if (ODmap[i]) free(ODmap[i]);
#endif
#if USE_ZMAP
		if (Zmap[i]) free(Zmap[i]);
#if USE_LDV
		if (OccZmap[i]) free(OccZmap[i]);
#endif
#endif
	}
	for (i = 0; i < 5; i++)
	{
		if (SynY[i]) free(SynY[i]);
		if (SynU[i]) free(SynU[i]);
		if (SynV[i]) free(SynV[i]);
#if USE_LDV
		if (OccY[i]) free(OccY[i]);
		if (OccU[i]) free(OccU[i]);
		if (OccV[i]) free(OccV[i]);
#endif
	}
}

void ViewSynthesis1D::SetPrecision(int precision)
{
	if (precision == 1)
	{
		m_subPelOption = 1;
		m_upsampleRefs = 1;
	}
	else if (precision == 2)
	{
		m_subPelOption = 2;
		m_upsampleRefs = 1;
	}
	else if (precision == 4)
	{
		m_subPelOption = 2;
		m_upsampleRefs = 2;
	}
	else
	{
		m_subPelOption = 1;
		m_upsampleRefs = 1;
		printf("Warning: the precision is not supported yet: %d\n", precision);
	}
}

int  ViewSynthesis1D::AllocMem()
{
	int i;
	size_t sz;

	m_width2 = m_width * m_upsampleRefs;

	//BoundaryMask[*] is now the size of Width2*Height (previously, Width*Height)
	sz = m_width2 * m_height;
	// When SplattingOption==2 or cfg.getCleanNoiseOption()==1
	if (cfg.getSplattingOption() == 2 || cfg.getCleanNoiseOption() == 1)
	{
		for (i = 0; i < 2; i++)
			if (BoundaryMask[i] == NULL)
			{
				BoundaryMask[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
				if (BoundaryMask[i] == NULL) return -1;
			}
	}

	sz = m_width2 * m_height*m_subPelOption;

	for (i = 0; i < 3; i++)
	{
		if (Mask[i] == NULL)
		{
			Mask[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (Mask[i] == NULL) return -1;
		}
		if (Dmap[i] == NULL)
		{
			Dmap[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (Dmap[i] == NULL) return -1;
		}
#if USE_LDV
		if (OMask[i] == NULL)
		{
			OMask[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (OMask[i] == NULL) return -1;
		}
		if (ODmap[i] == NULL)
		{
			ODmap[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (Dmap[i] == NULL) return -1;
		}
#endif
#if USE_ZMAP
		if (Zmap[i] == NULL)
		{
			Zmap[i] = (float*)malloc(sz * sizeof(float));
			if (Zmap[i] == NULL) return -1;
		}
#if USE_LDV
		if (OccZmap[i] == NULL)
		{
			OccZmap[i] = (float*)malloc(sz * sizeof(float));
			if (OccZmap[i] == NULL) return -1;
		}
#endif
#endif
	}

	for (i = 0; i < 4; i++)
	{
		if (SynY[i] == NULL)
		{
			SynY[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (SynY[i] == NULL) return -1;
		}
		if (SynU[i] == NULL)
		{
			SynU[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (SynU[i] == NULL) return -1;
		}
		if (SynV[i] == NULL)
		{
			SynV[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (SynV[i] == NULL) return -1;
		}
#if USE_LDV
		if (OccY[i] == NULL)
		{
			OccY[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (OccY[i] == NULL) return -1;
		}
		if (OccU[i] == NULL)
		{
			OccU[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (OccU[i] == NULL) return -1;
		}
		if (OccV[i] == NULL)
		{
			OccV[i] = (unsigned char*)malloc(sz * sizeof(unsigned char));
			if (OccV[i] == NULL) return -1;
		}
#endif
	}

	// Scaled down version
	sz = m_width2 * m_height;
	if (SynY[FINLVIEW] == NULL)
	{
		SynY[FINLVIEW] = (unsigned char*)malloc(sz * sizeof(unsigned char));
		if (SynY[FINLVIEW] == NULL) return -1;
	}
	if (SynU[FINLVIEW] == NULL)
	{
		SynU[FINLVIEW] = (unsigned char*)malloc(sz * sizeof(unsigned char));
		if (SynU[FINLVIEW] == NULL) return -1;
	}
	if (SynV[FINLVIEW] == NULL)
	{
		SynV[FINLVIEW] = (unsigned char*)malloc(sz * sizeof(unsigned char));
		if (SynV[FINLVIEW] == NULL) return -1;
	}
#if USE_LDV
	if (OccY[FINLVIEW] == NULL)
	{
		OccY[FINLVIEW] = (unsigned char*)malloc(sz * sizeof(unsigned char));
		if (OccY[FINLVIEW] == NULL) return -1;
	}
	if (OccU[FINLVIEW] == NULL)
	{
		OccU[FINLVIEW] = (unsigned char*)malloc(sz * sizeof(unsigned char));
		if (OccU[FINLVIEW] == NULL) return -1;
	}
	if (OccV[FINLVIEW] == NULL)
	{
		OccV[FINLVIEW] = (unsigned char*)malloc(sz * sizeof(unsigned char));
		if (OccV[FINLVIEW] == NULL) return -1;
	}
#endif
	if (m_upsampleRefs != 1)
	{
		for (i = 0; i < 2; i++)
		{
			if (RefView[i] == NULL)
			{
				RefView[i] = (ImageType*)malloc(sz * 3 * sizeof(ImageType));
				if (RefView[i] == NULL) return -1;
			}
			if (RefDepth[i] == NULL)
			{
				RefDepth[i] = (DepthType*)malloc(sz * sizeof(DepthType));  // ???????????
				if (RefDepth[i] == NULL) return -1;
			}
		}
	}

	//Alloc Memory for TIM and cleaning boundary noise, Zhejiang
	sz = m_width2 * m_height;
	for (i = 0; i < 2; i++)
	{
		if (RefViewLast[i] == NULL)
		{
			RefViewLast[i] = (ImageType*)malloc(sz * sizeof(ImageType));
			if (RefViewLast[i] == NULL) return -1;
		}
		if (RefDepthLast[i] == NULL)
		{
			RefDepthLast[i] = (DepthType*)malloc(sz * sizeof(DepthType));  // ???????????
			if (RefDepthLast[i] == NULL) return -1;
		}
	}

	WeightLeft = fabs(LTranslation[RGHTVIEW] / (fabs(LTranslation[LEFTVIEW]) + fabs(LTranslation[RGHTVIEW])));
	WeightRight = 1 - WeightLeft;
	return 0;
}

/*
 * \brief
 *    Map a pixel to the virtual view using the z buffer
 *
 * \input
 * \param x, y
 *    The image cooredinate of the pixel to be mapped
 * \param d
 *    The depth level of the pixel to be mapped
 * \param z
 *    The depth value of the pixel to be mapped
 * \param RefY
 *    The reference view buffer, Y. Note that UV components are stored in RefU[ViewId] and RefV[ViewId]
 * \param ViewId
 *    0 means left view; 1 means right view
 * \param dk
 *    The disparity of the pixel to be mapped
 * \param flooring
 *    Indicate if floor() or ceil() to be used in the mapping process
 *
 * \output
 * \class member SynY[ViewId], SynU[ViewId], SynV[ViewId]
 *    The warped picture is stored. 4:4:4
 *
 * \return
 *    None
 *
 * \Modification history:
 *    \Zhejiang Univ, May 2009
 *        Bug fix on pixel mapping
 *        Must work together with new version ForwardWarpSingleView() function as it need a correct warping direction.
 */
void ViewSynthesis1D::PixelMapping(int x, int y, unsigned char d, float z, unsigned char* RefY, unsigned char* RefU, unsigned char* RefV, int ViewId, double dk, int flooring)
{
	int   x2, y2;
	int   ind, ind2;
	bool  nearToCamera;
	bool  bSplatting;
#if !VSRS3_ORIGINAL
	int   left_pixel, right_pixel, left2_pixel, right2_pixel;
#endif
	int   line_size = m_width2 * m_subPelOption;
	int   enable_warp_flag = 2;
	int   depth_diff_th = 10;
	int   average_depth = 0;

	// ind = (x,y) is the pixel position in ref view
	ind = x + y * m_width2;

	// Do splatting?
	if (cfg.getSplattingOption() == 0)
		bSplatting = false;
	else if (cfg.getSplattingOption() == 1)
		bSplatting = true;
	else if (cfg.getSplattingOption() == 2)
	{
		// check if the ref pixel is boundary, revised by Yin (SplattingDepthThreshold: a sequence adaptive parameter)
		if (d < SplattingDepthThreshold || IsBoundary(BoundaryMask[ViewId], x, y) || x < 30 || x > m_width2 - 30)
			bSplatting = true;
		else
			bSplatting = false;
	}

	// Calc the position in syn view
	if (bSplatting)
	{
		if (flooring)
			x2 = (int)(floor((x - dk) * m_subPelOption));
		else
			x2 = (int)(ceil((x - dk) * m_subPelOption));
	}
	else
		x2 = (int)(floor((x - dk) * m_subPelOption + 0.5));

	y2 = y;

	if (x2 < line_size && x2 >= 0 &&
		y2 < m_height              && y2 >= 0) // (x2,y2) must fall within the picture
	{
		ind2 = x2 + y2 * line_size;
		nearToCamera = false;
		if (Mask[ViewId][ind2] == NOTHOLEPIXEL)
		{
#if USE_ZMAP
			if (Znear[LEFTVIEW] > 0 && Zmap[ViewId][ind2] > z)
				nearToCamera = true;
			if (Znear[LEFTVIEW] < 0 && Zmap[ViewId][ind2] < z)
				nearToCamera = true;

			if (Dmap[ViewId][ind2] < d - 3)  //if depth value of current pixel is larger, it covers the previous result
				nearToCamera = true;
#else
			if (Dmap[ViewId][ind2] < d)  //if depth value of current pixel is larger, it covers the previous result
				nearToCamera = true;
#endif
		}

		//pixel mapping
#if VSRS3_ORIGINAL
		if (Mask[ViewId][ind2] == HOLEPIXEL || // (x2,y2) is not filled with any value yet
			nearToCamera) // (x2,y2) was filled, but the new pixel is near to the camera  Mask[ViewId][ind2]==NOTHOLEPIXEL && Zmap[ViewId][ind2] > z
		{
			SynY[ViewId][ind2] = RefY[ind];
			SynU[ViewId][ind2] = RefU[ind];
			SynV[ViewId][ind2] = RefV[ind];
			Mask[ViewId][ind2] = NOTHOLEPIXEL;
			Dmap[ViewId][ind2] = d;
#if USE_ZMAP
			Zmap[ViewId][ind2] = z;
#endif
		}
#else // related to "VSRS3_ORIGINAL", And in current version, LDV data generation is not supported
	//for HOLEPIXEL, revised to clean noise background pixels
		if (Mask[ViewId][ind2] == HOLEPIXEL)// (x2,y2) is not filled with any value yet, should notice it is not a background pixel
		{
			//constrain
			left_pixel = CLIP(x2 - 1, 0, line_size - 1) + y2 * line_size;
			left2_pixel = CLIP(x2 - 2, 0, line_size - 1) + y2 * line_size;
			right_pixel = CLIP(x2 + 1, 0, line_size - 1) + y2 * line_size;
			right2_pixel = CLIP(x2 + 2, 0, line_size - 1) + y2 * line_size;

			//filter wrong background pixels caused by per-pixel processing
			//do not warp the pixel, because it is a suspicious pixel in the gap of foreground object
			if (Mask[ViewId][left_pixel] == NOTHOLEPIXEL && Mask[ViewId][right_pixel] == NOTHOLEPIXEL && cfg.getWarpEnhancementOption() == 1)
			{
				average_depth = (Dmap[ViewId][left_pixel] + Dmap[ViewId][right_pixel]) / 2;
				depth_diff_th = (int)((average_depth - Minz)*0.05 + 0.5);
				if (abs(Dmap[ViewId][left_pixel] - Dmap[ViewId][right_pixel]) < depth_diff_th && abs(d - average_depth) > depth_diff_th)
				{
					enable_warp_flag = 0;
					SynY[ViewId][ind2] = (int)((SynY[ViewId][left_pixel] + SynY[ViewId][right_pixel]) / 2);
					SynU[ViewId][ind2] = (int)((SynU[ViewId][left_pixel] + SynU[ViewId][right_pixel]) / 2);
					SynV[ViewId][ind2] = (int)((SynV[ViewId][left_pixel] + SynV[ViewId][right_pixel]) / 2);
					Mask[ViewId][ind2] = NOTHOLEPIXEL;
					Dmap[ViewId][ind2] = (int)((Dmap[ViewId][left_pixel] + Dmap[ViewId][right_pixel]) / 2);
					CountHOLEFILLPIXEL++;
#if USE_ZMAP
					Zmap[ViewId][ind2] = (int)((Zmap[ViewId][left_pixel] + Zmap[ViewId][right_pixel]) / 2);
#endif
				}
				else
					enable_warp_flag = 1;
			}
			else if (Mask[ViewId][left_pixel] == NOTHOLEPIXEL && Mask[ViewId][right2_pixel] == NOTHOLEPIXEL && cfg.getWarpEnhancementOption() == 1)
			{
				average_depth = (Dmap[ViewId][left_pixel] + Dmap[ViewId][right2_pixel]) / 2;
				depth_diff_th = (int)((average_depth - Minz)*0.08 + 0.5);
				if (abs(Dmap[ViewId][left_pixel] - Dmap[ViewId][right2_pixel]) < depth_diff_th && abs(d - average_depth) > depth_diff_th / 2)
				{
					enable_warp_flag = 0;
					SynY[ViewId][ind2] = (int)((SynY[ViewId][left_pixel] + SynY[ViewId][right2_pixel]) / 2);
					SynU[ViewId][ind2] = (int)((SynU[ViewId][left_pixel] + SynU[ViewId][right2_pixel]) / 2);
					SynV[ViewId][ind2] = (int)((SynV[ViewId][left_pixel] + SynV[ViewId][right2_pixel]) / 2);
					Mask[ViewId][ind2] = NOTHOLEPIXEL;
					Dmap[ViewId][ind2] = (int)((Dmap[ViewId][left_pixel] + Dmap[ViewId][right2_pixel]) / 2);
					CountHOLEFILLPIXEL++;
#if USE_ZMAP
					Zmap[ViewId][ind2] = (int)((Zmap[ViewId][left_pixel] + Zmap[ViewId][right2_pixel]) / 2);
#endif
				}
				else
					enable_warp_flag = 1;
			}
			else if (Mask[ViewId][left2_pixel] == NOTHOLEPIXEL && Mask[ViewId][right_pixel] == NOTHOLEPIXEL && cfg.getWarpEnhancementOption() == 1)
			{
				average_depth = (Dmap[ViewId][left2_pixel] + Dmap[ViewId][right_pixel]) / 2;
				depth_diff_th = (int)((average_depth - Minz)*0.08 + 0.5);
				if (abs(Dmap[ViewId][left2_pixel] - Dmap[ViewId][right_pixel]) < depth_diff_th && abs(d - average_depth) > depth_diff_th / 2)
				{
					enable_warp_flag = 0;
					SynY[ViewId][ind2] = (int)((SynY[ViewId][left2_pixel] + SynY[ViewId][right_pixel]) / 2);
					SynU[ViewId][ind2] = (int)((SynU[ViewId][left2_pixel] + SynU[ViewId][right_pixel]) / 2);
					SynV[ViewId][ind2] = (int)((SynV[ViewId][left2_pixel] + SynV[ViewId][right_pixel]) / 2);
					Mask[ViewId][ind2] = NOTHOLEPIXEL;
					Dmap[ViewId][ind2] = (int)((Dmap[ViewId][left2_pixel] + Dmap[ViewId][right_pixel]) / 2);
					CountHOLEFILLPIXEL++;
#if USE_ZMAP
					Zmap[ViewId][ind2] = (int)((Zmap[ViewId][left2_pixel] + Zmap[ViewId][right_pixel]) / 2);
#endif
				}
				else
					enable_warp_flag = 1;
			}
			else
				enable_warp_flag = 1;

			//enable to warp current pixel if the pixel is not belong to the area around strong depth edge
			//aiming to clean boundary noise causing by slow depth change or depth edge be a little inside of texture edge
			if (enable_warp_flag == 1)
			{
				//if the pixel is at left side of a RISE depth edge or at right side of a FALL depth edge, do not warp the pixel
				if (cfg.getSplattingOption() != 2 && cfg.getCleanNoiseOption() != 1)  // bug fixed July 16th: when cfg.getSplattingOption() == 0 or 1 && cfg.getCleanNoiseOption() == 0, no boundary detection was made
				{
					SynY[ViewId][ind2] = RefY[ind];
					SynU[ViewId][ind2] = RefU[ind];
					SynV[ViewId][ind2] = RefV[ind];
					Mask[ViewId][ind2] = NOTHOLEPIXEL;
					Dmap[ViewId][ind2] = d;
#if USE_ZMAP
					Zmap[ViewId][ind2] = z;
#endif
				}
				else  //Boundary aware splatting
				{
					if (BoundaryMask[ViewId][ind] == SUSPECT && cfg.getCleanNoiseOption() == 1)
						;//do nothing
					else
					{
						SynY[ViewId][ind2] = RefY[ind];
						SynU[ViewId][ind2] = RefU[ind];
						SynV[ViewId][ind2] = RefV[ind];
						Mask[ViewId][ind2] = NOTHOLEPIXEL;
						Dmap[ViewId][ind2] = d;
#if USE_ZMAP
						Zmap[ViewId][ind2] = z;
#endif
					}
				}
			}
		}//end of if( Mask[ViewId][ind2] == HOLEPIXEL )
		else if (nearToCamera) // (x2,y2) was filled, but the new pixel is near to the camera  Mask[ViewId][ind2]==NOTHOLEPIXEL && Zmap[ViewId][ind2] > z )
		{
			if (cfg.getSplattingOption() != 2 && cfg.getCleanNoiseOption() != 1)  // bug fixed July 16th: when cfg.getSplattingOption() == 0 or 1 && cfg.getCleanNoiseOption() == 0 , no boundary detection was made
			{
				SynY[ViewId][ind2] = RefY[ind];
				SynU[ViewId][ind2] = RefU[ind];
				SynV[ViewId][ind2] = RefV[ind];
				Mask[ViewId][ind2] = NOTHOLEPIXEL;
				Dmap[ViewId][ind2] = d;
#if USE_ZMAP
				Zmap[ViewId][ind2] = z;
#endif
			}
			else
			{
				if (BoundaryMask[ViewId][ind] == SUSPECT && cfg.getCleanNoiseOption() == 1) //
					;//do nothing
				else
				{
					SynY[ViewId][ind2] = RefY[ind];
					SynU[ViewId][ind2] = RefU[ind];
					SynV[ViewId][ind2] = RefV[ind];
					Mask[ViewId][ind2] = NOTHOLEPIXEL;
					Dmap[ViewId][ind2] = d;
#if USE_ZMAP
					Zmap[ViewId][ind2] = z;
#endif
				}
			}
		}
#endif
	}//end of if ( x2<line_size && x2>=0 ...)
}

/*
 * \brief
 *    Warp one view
 *
 * \input
 * \param Ref
 *    The reference view buffer
 * \param RefDepth
 *    The reference depth view buffer
 * \param ViewId
 *    0 means left view; 1 means right view
 *
 * \output
 * \class member SynY[ViewId], SynU[ViewId], SynV[ViewId]
 *    The warped picture is stored. 4:4:4
 *
 * \return
 *    None
 *
 * \Modification history:
 *    \Zhejiang Univ, May 2009
 *      Warping direction consideration
 *      Change the warping order: if Warp to right side, pixel mapping from left to right; otherwise, the opposite.
 */
void ViewSynthesis1D::ForwardWarpSingleView(int ViewId)
{
	int x, y;
	int ind;
	int depthLevel;
	double dk;
	double z;
	unsigned char*  RefY = &RefView[ViewId][0];
	unsigned char*  RefU = &RefView[ViewId][m_width2 * m_height];
	unsigned char*  RefV = &RefView[ViewId][m_width2 * m_height * 2];

	CountHOLEFILLPIXEL = 0;
	//determine warp direction
	if (cfg.getWarpEnhancementOption() == 0)
		WarpToRight = 0;
	else
	{
		if (ViewId == LEFTVIEW)
			WarpToRight = 1;
		else
			WarpToRight = 0;
	}

	//start to warp each pixel
	if (WarpToRight == 0) // left view-- from left 2 right
		for (y = 0; y < m_height; y++)
			for (x = 0; x < m_width2; x++)
			{
				// ind = (x,y) is the pixel position in ref view
				ind = x + y * m_width2;
				// Calc real depth z from depth image
				depthLevel = RefDepth[ViewId][ind];
				z = 1.0 / ((depthLevel / (MaxTypeValue<DepthType>() - 1.0)) * (1 / Znear[ViewId] - 1 / Zfar[ViewId]) + (1 / Zfar[ViewId]));

				// Cacl dk; round to integer pixel postionns;
				dk = (cfg.getFocalLength() * LTranslation[ViewId] / z) - duPrincipal[ViewId];

				if (m_upsampleRefs == 2)
					dk = dk + dk;
				else if (m_upsampleRefs == 4)
					dk = dk * 4;

				PixelMapping(x, y, RefDepth[ViewId][ind], (float)z, RefY, RefU, RefV, ViewId, dk, 1); //09-02-25
				if (cfg.getSplattingOption())
					PixelMapping(x, y, RefDepth[ViewId][ind], (float)z, RefY, RefU, RefV, ViewId, dk, 0);
			}
	else  //if(WarpToRight == 1) // right view-- from right 2 left
		for (y = 0; y < m_height; y++)
			for (x = m_width2 - 1; x >= 0; x--)
			{
				// ind = (x,y) is the pixel position in ref view
				ind = x + y * m_width2;
				// Calc real depth z from depth image
				depthLevel = RefDepth[ViewId][ind];
				z = 1.0 / ((depthLevel / (MaxTypeValue<DepthType>() - 1.0)) * (1 / Znear[ViewId] - 1 / Zfar[ViewId]) + (1 / Zfar[ViewId]));

				// Cacl dk; round to integer pixel postionns;
				dk = (cfg.getFocalLength() * LTranslation[ViewId] / z) - duPrincipal[ViewId];

				if (m_upsampleRefs == 2)
					dk = dk + dk;
				else if (m_upsampleRefs == 4)
					dk = dk * 4;

				PixelMapping(x, y, RefDepth[ViewId][ind], (float)z, RefY, RefU, RefV, ViewId, dk, 0);  //Note: here must be flooring == 0 first
				if (cfg.getSplattingOption())
					PixelMapping(x, y, RefDepth[ViewId][ind], (float)z, RefY, RefU, RefV, ViewId, dk, 1);
			}

#if DEBUG_ZHEJIANG
	if (cfg.getWarpEnhancementOption() == 1)
	{
		if (ViewId == LEFTVIEW)
			fprintf(stderr, "\n Count Holefill pixel in LEFT View 1D warp = %d", CountHOLEFILLPIXEL);
		else
			fprintf(stderr, "\n Count Holefill pixel in RIGHT View 1D warp = %d", CountHOLEFILLPIXEL);
	}
#endif
}

/*
 * \brief
 *    Perform forward warping
 *
 * \input
 *    The images are arranged in a one dimension array
 * \param RefLeft
 *    The left ref view image
 * \param RefRight
 *    The right ref view image
 * \param RefDepthLeft
 *    The left ref depth view image
 * \param RefDepthRight
 *    The right ref depth view image
 *
 * \output
 * \class member: SynY[i], SynU[i], SynV[i]
 *    Warped view
 *    i = 0 is from left view
 *    i = 1 is from right view
 *  \class member: Mask[i]
 *    Hole positions
 *    i = 0 is from left view
 *    i = 1 is from right view
 *  \class member: Zmap[i]
 *    The depth value of warped pixels
 *    i = 0 is from left view
 *    i = 1 is from right view
 *  \class member: Dmap[i]
 *    The depth level of warped pixels
 *    i = 0 is from left view
 *    i = 1 is from right view
 *
 * \return
 *    None
 */
void ViewSynthesis1D::ForwardWarp() // unsigned char* RefLeft, unsigned char* RefRight, unsigned char* RefDepthLeft, unsigned char* RefDepthRight
{
	int i;

	for (i = 0; i < 3; i++)
	{
		memset(SynY[i], 0, m_width2*m_height*m_subPelOption);
		memset(SynU[i], 128, m_width2*m_height*m_subPelOption);
		memset(SynV[i], 128, m_width2*m_height*m_subPelOption);
		memset(Mask[i], HOLEPIXEL, m_width2*m_height*m_subPelOption);
		memset(Dmap[i], 0, m_width2*m_height*m_subPelOption * sizeof(unsigned char));
#if USE_LDV
		memset(OccY[i], 0, Width2*Height*SubPelOption);
		memset(OccU[i], 128, Width2*Height*SubPelOption);
		memset(OccV[i], 128, Width2*Height*SubPelOption);
		memset(OMask[i], HOLEPIXEL, Width2*Height*SubPelOption);
		memset(ODmap[i], 0, Width2*Height*SubPelOption * sizeof(unsigned char));
#endif
#if USE_ZMAP
		memset(Zmap[i], 0, m_width2*m_height*m_subPelOption * sizeof(float));
#if USE_LDV
		memset(OccZmap[i], 0, Width2*Height*SubPelOption * sizeof(float));
#endif
#endif
	}

	//find out the max and min z value in current frame
	FindDepthMaxMin(RefDepth[LEFTVIEW], LEFTVIEW); //left view must have a similar depth structure as that of the right view

	//depth map edge detection before forward warping if cfg.getSplattingOption()==2 or cfg.getCleanNoiseOption()==1
	if (cfg.getSplattingOption() == 2 || cfg.getCleanNoiseOption() == 1)
	{
		double es = 0.15; //EdgeStrength
		int diff_z = Maxz - Minz;
		int th = (int)(es*diff_z / m_upsampleRefs + 1); //depth edge threshold = 15% * z_value_range / UpsampleRefs

		for (i = 0; i < 2; i++)
		{
			memset(BoundaryMask[i], NONE, m_width2*m_height);
			DetectBoundary(BoundaryMask[i], RefDepth[i], m_width2, m_height, th);
		}
	}

	//forward warping left and right view
	// From Left View
	ForwardWarpSingleView(LEFTVIEW); // RefLeft,  RefDepthLeft,

	// From Right View
	ForwardWarpSingleView(RGHTVIEW); // RefRight, RefDepthRight,
}

/*
 * \brief
 *    Count the number of hole pixels around the specified position
 *
 * \input
 * \param MaskMap
 *    The hole mask tabl
 * \param x, y
 *    Specify the position to count number of holes
 * \param width
 *    The width of the picture
 *
 * \return
 *    Number of hole pixels
 */
int ViewSynthesis1D::CountHolePixels(unsigned char* MaskMap, int x, int y, int width)
{
	int blksizey = 3;
	int blksizex = blksizey * m_subPelOption*m_upsampleRefs;
	int startx, endx;
	int starty, endy;
	int i, j;
	int counter = 0;
	int ind;

	startx = x - blksizex;
	endx = x + blksizex;
	starty = y - blksizey;
	endy = y + blksizey;

	if (startx < 0) startx = 0;
	if (endx > width) endx = width;
	if (starty < 0) starty = 0;
	if (endy > m_height)  endy = m_height;

	for (j = starty; j < endy; j++)
	{
		ind = j * width + startx;
		for (i = startx; i < endx; i++)
		{
			counter += MaskMap[ind];
			ind++;
		}
	}

	return counter;
}

/*
 * \brief
 *    Merge the two synthesized view into one
 *
 * \input
 * \class member SynY[i], SynU[i], SynV[i]
 *    The synthesized view, in 4:4:4
 *    i = 0 from left view
 *    i = 1 from right view
 * \class member Mask[i]
 *    The hole map
 *    i = 0 from left view
 *    i = 1 from right view
 * \class member Zmap[i]
 *    The depth map (real depth values stored) warped from
 *    i = 0 from left view
 *    i = 1 from right view
 * \class member Dmap[i]
 *    The depth map (depth levels stored) warped from
 *    i = 0 from left view
 *    i = 1 from right view
 *
 * \output
 * \class member SynY[MERGVIEW], SynU[MERGVIEW], SynV[MERGVIEW]
 *    The unique synthesized picture, in 4:4:4
 * \class member Mask[MERGVIEW]
 *    The unique hole mask
 * \class member Zmap[MERGVIEW]
 *    The unique synthesized depth map (real depth values stored)
 * \class member Dmap[MERGVIEW]
 *    The unique synthesized depth map (depth levels stored)
 *
 * \return
 *    None
 */
void ViewSynthesis1D::Merge()
{
	int x, y;
	int ind;
	double Y, U, V, D;
#if USE_ZMAP
	double Z;
#endif
	int HoleCountLeft, HoleCountRight;
	bool isLeftNearToCamera = true;  // just to avoid compile warning
	int WidthSyn = m_width2 * m_subPelOption;

	ind = 0;
	for (y = 0; y < m_height; y++)
		for (x = 0; x < WidthSyn; x++)
		{
			// The current pixel has two versions
			if (Mask[LEFTVIEW][ind] == NOTHOLEPIXEL && Mask[RGHTVIEW][ind] == NOTHOLEPIXEL)
			{
				if (cfg.getMergingOption() == 0)
				{ // Z-buffer only
#if USE_ZMAP
					if (Znear[LEFTVIEW] > 0)
					{
						if (Zmap[LEFTVIEW][ind] <= Zmap[RGHTVIEW][ind])
							isLeftNearToCamera = true;
						else
							isLeftNearToCamera = false;
					}
					else
					{
						if (Zmap[LEFTVIEW][ind] >= Zmap[RGHTVIEW][ind])
							isLeftNearToCamera = true;
						else
							isLeftNearToCamera = false;
					}
#else
					if (Dmap[LEFTVIEW][ind] >= Dmap[RGHTVIEW][ind])
						isLeftNearToCamera = true;
					else
						isLeftNearToCamera = false;
#endif
					if (isLeftNearToCamera)
					{    // Left view is near to the camera
						SynY[MERGVIEW][ind] = SynY[LEFTVIEW][ind];
						SynU[MERGVIEW][ind] = SynU[LEFTVIEW][ind];
						SynV[MERGVIEW][ind] = SynV[LEFTVIEW][ind];
						Dmap[MERGVIEW][ind] = Dmap[LEFTVIEW][ind];
						Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
						Zmap[MERGVIEW][ind] = Zmap[LEFTVIEW][ind];
#endif
					}
					// Right view is near to the camera
					else
					{
						SynY[MERGVIEW][ind] = SynY[RGHTVIEW][ind];
						SynU[MERGVIEW][ind] = SynU[RGHTVIEW][ind];
						SynV[MERGVIEW][ind] = SynV[RGHTVIEW][ind];
						Dmap[MERGVIEW][ind] = Dmap[RGHTVIEW][ind];
						Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
						Zmap[MERGVIEW][ind] = Zmap[RGHTVIEW][ind];
#endif
					}
				}
				else if (cfg.getMergingOption() == 1)
				{// Camera distance as weighting factor
					Y = WeightLeft * (double)SynY[LEFTVIEW][ind] + WeightRight * (double)SynY[RGHTVIEW][ind];
					U = WeightLeft * (double)SynU[LEFTVIEW][ind] + WeightRight * (double)SynU[RGHTVIEW][ind];
					V = WeightLeft * (double)SynV[LEFTVIEW][ind] + WeightRight * (double)SynV[RGHTVIEW][ind];
#if USE_ZMAP
					Z = WeightLeft * (double)Zmap[LEFTVIEW][ind] + WeightRight * (double)Zmap[RGHTVIEW][ind];
#endif
					D = WeightLeft * (double)Dmap[LEFTVIEW][ind] + WeightRight * (double)Dmap[RGHTVIEW][ind];
					SynY[MERGVIEW][ind] = (unsigned char)Y;
					SynU[MERGVIEW][ind] = (unsigned char)U;
					SynV[MERGVIEW][ind] = (unsigned char)V;
					Dmap[MERGVIEW][ind] = (unsigned char)D;
					Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
					Zmap[MERGVIEW][ind] = (unsigned char)Z;
#endif
				}

				else if (cfg.getMergingOption() == 2)
				{// Hole counting + Z-buffer
				  // Note that because the DE s/w currently output diff depth range for diff views, the performance may be a little worse than expected

				  // When the two pixel depthes are close enough, we may do average
					if (abs(Dmap[LEFTVIEW][ind] - Dmap[RGHTVIEW][ind]) <= cfg.getDepthThreshold()) //camera weighting         Dmap[RGHTVIEW][ind]
					{
						//hole counting
						HoleCountLeft = CountHolePixels(Mask[LEFTVIEW], x, y, WidthSyn);
						HoleCountRight = CountHolePixels(Mask[RGHTVIEW], x, y, WidthSyn);
						bool One_view = (abs(HoleCountLeft - HoleCountRight) >= cfg.getHoleCountThreshold()*m_subPelOption*m_upsampleRefs);
						//One_view = false;

						//bool One_view;
						//if (IsBoundary(BoundaryMask[LEFTVIEW], x, y) || IsBoundary(BoundaryMask[RGHTVIEW], x, y))
						// If it is obvious that one synthesized result contains much less holes, it is assumed to be more reliable
						if (One_view)
						{
							if (HoleCountLeft <= HoleCountRight)
							{ //pick left, sine less holes from left
								SynY[MERGVIEW][ind] = SynY[LEFTVIEW][ind];
								SynU[MERGVIEW][ind] = SynU[LEFTVIEW][ind];
								SynV[MERGVIEW][ind] = SynV[LEFTVIEW][ind];
								Dmap[MERGVIEW][ind] = Dmap[LEFTVIEW][ind];
								Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
								Zmap[MERGVIEW][ind] = Zmap[LEFTVIEW][ind];
#endif
							}
							else
							{//pick right, since less holes from right
								SynY[MERGVIEW][ind] = SynY[RGHTVIEW][ind];
								SynU[MERGVIEW][ind] = SynU[RGHTVIEW][ind];
								SynV[MERGVIEW][ind] = SynV[RGHTVIEW][ind];
								Dmap[MERGVIEW][ind] = Dmap[RGHTVIEW][ind];
								Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
								Zmap[MERGVIEW][ind] = Zmap[RGHTVIEW][ind];
#endif
							}
						}

						// Otherwise, do average
						else //camera distance weighting
						{
							Y = WeightLeft * (double)SynY[LEFTVIEW][ind] + WeightRight * (double)SynY[RGHTVIEW][ind];
							U = WeightLeft * (double)SynU[LEFTVIEW][ind] + WeightRight * (double)SynU[RGHTVIEW][ind];
							V = WeightLeft * (double)SynV[LEFTVIEW][ind] + WeightRight * (double)SynV[RGHTVIEW][ind];
#if USE_ZMAP
							Z = WeightLeft * (double)Zmap[LEFTVIEW][ind] + WeightRight * (double)Zmap[RGHTVIEW][ind];
#endif
							D = WeightLeft * (double)Dmap[LEFTVIEW][ind] + WeightRight * (double)Dmap[RGHTVIEW][ind];
							SynY[MERGVIEW][ind] = (unsigned char)Y;
							SynU[MERGVIEW][ind] = (unsigned char)U;
							SynV[MERGVIEW][ind] = (unsigned char)V;
							Dmap[MERGVIEW][ind] = (unsigned char)D;
							Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
							Zmap[MERGVIEW][ind] = (float)Z;
#endif
						}
					}
					// When the two pixel depthes are far away enough, we always select one winner
					else
					{
						// find out which is near to the camera
#if USE_ZMAP
						if (Znear[LEFTVIEW] > 0)
						{
							if (Zmap[LEFTVIEW][ind] <= Zmap[RGHTVIEW][ind])
								isLeftNearToCamera = true;
							else
								isLeftNearToCamera = false;
						}
						else
						{
							if (Zmap[LEFTVIEW][ind] >= Zmap[RGHTVIEW][ind])
								isLeftNearToCamera = true;
							else
								isLeftNearToCamera = false;
						}
#else
						if (Dmap[LEFTVIEW][ind] >= Dmap[RGHTVIEW][ind])
							isLeftNearToCamera = true;
						else
							isLeftNearToCamera = false;
#endif
						if (isLeftNearToCamera)
						{    // Left view is near to the camera
							SynY[MERGVIEW][ind] = SynY[LEFTVIEW][ind];
							SynU[MERGVIEW][ind] = SynU[LEFTVIEW][ind];
							SynV[MERGVIEW][ind] = SynV[LEFTVIEW][ind];
#if USE_ZMAP
							Zmap[MERGVIEW][ind] = Zmap[LEFTVIEW][ind];
#endif
							Dmap[MERGVIEW][ind] = Dmap[LEFTVIEW][ind];
							Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
						}
						else
						{// Right view is near to the camera
							SynY[MERGVIEW][ind] = SynY[RGHTVIEW][ind];
							SynU[MERGVIEW][ind] = SynU[RGHTVIEW][ind];
							SynV[MERGVIEW][ind] = SynV[RGHTVIEW][ind];
							Dmap[MERGVIEW][ind] = Dmap[RGHTVIEW][ind];
							Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
							Zmap[MERGVIEW][ind] = Zmap[RGHTVIEW][ind];
#endif
						}
					}
				}
#if USE_ZMAP
#if USE_LDV
				if (Znear[LEFTVIEW] > 0)
				{
					if (OccZmap[LEFTVIEW][ind] <= OccZmap[RGHTVIEW][ind])
						isLeftNearToCamera = true;
					else
						isLeftNearToCamera = false;
				}
				else
				{
					if (OccZmap[LEFTVIEW][ind] >= OccZmap[RGHTVIEW][ind])
						isLeftNearToCamera = true;
					else
						isLeftNearToCamera = false;
				}
#endif
#else
				if (ODmap[LEFTVIEW][ind] >= ODmap[RGHTVIEW][ind])
					isLeftNearToCamera = true;
				else
					isLeftNearToCamera = false;
#endif

#if USE_LDV
				if (isLeftNearToCamera)
				{    // Right view is far from the camera
					OccY[MERGVIEW][ind] = OccY[RGHTVIEW][ind];
					OccU[MERGVIEW][ind] = OccU[RGHTVIEW][ind];
					OccV[MERGVIEW][ind] = OccV[RGHTVIEW][ind];
					ODmap[MERGVIEW][ind] = ODmap[RGHTVIEW][ind];
					OMask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
					OccZmap[MERGVIEW][ind] = OccZmap[RGHTVIEW][ind];
#endif
				}
				// Left view is far from the camera
				else
				{
					OccY[MERGVIEW][ind] = OccY[LEFTVIEW][ind];
					OccU[MERGVIEW][ind] = OccU[LEFTVIEW][ind];
					OccV[MERGVIEW][ind] = OccV[LEFTVIEW][ind];
					ODmap[MERGVIEW][ind] = ODmap[LEFTVIEW][ind];
					OMask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
					OccZmap[MERGVIEW][ind] = OccZmap[LEFTVIEW][ind];
#endif
				}
#endif
			}
			// The current pixel only has one version from the left view
			else if (Mask[LEFTVIEW][ind] == NOTHOLEPIXEL)
			{
				SynY[MERGVIEW][ind] = SynY[LEFTVIEW][ind];
				SynU[MERGVIEW][ind] = SynU[LEFTVIEW][ind];
				SynV[MERGVIEW][ind] = SynV[LEFTVIEW][ind];
				Dmap[MERGVIEW][ind] = Dmap[LEFTVIEW][ind];
				Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
				Zmap[MERGVIEW][ind] = Zmap[LEFTVIEW][ind];
#endif

#if USE_LDV
				OccY[MERGVIEW][ind] = OccY[LEFTVIEW][ind];
				OccU[MERGVIEW][ind] = OccU[LEFTVIEW][ind];
				OccV[MERGVIEW][ind] = OccV[LEFTVIEW][ind];
				ODmap[MERGVIEW][ind] = ODmap[LEFTVIEW][ind];
				OMask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
				OccZmap[MERGVIEW][ind] = OccZmap[LEFTVIEW][ind];
#endif
#endif
			}
			// The current pixel only has one version from the right view
			else if (Mask[RGHTVIEW][ind] == NOTHOLEPIXEL)
			{
				SynY[MERGVIEW][ind] = SynY[RGHTVIEW][ind];
				SynU[MERGVIEW][ind] = SynU[RGHTVIEW][ind];
				SynV[MERGVIEW][ind] = SynV[RGHTVIEW][ind];
				Dmap[MERGVIEW][ind] = Dmap[RGHTVIEW][ind];
				Mask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
				Zmap[MERGVIEW][ind] = Zmap[RGHTVIEW][ind];
#endif
#if USE_LDV
				OccY[MERGVIEW][ind] = OccY[RGHTVIEW][ind];
				OccU[MERGVIEW][ind] = OccU[RGHTVIEW][ind];
				OccV[MERGVIEW][ind] = OccV[RGHTVIEW][ind];
				ODmap[MERGVIEW][ind] = ODmap[RGHTVIEW][ind];
				OMask[MERGVIEW][ind] = NOTHOLEPIXEL;
#if USE_ZMAP
				OccZmap[MERGVIEW][ind] = OccZmap[RGHTVIEW][ind];
#endif
#endif
			}

#if USE_LDV
			if (ODmap[MERGVIEW][ind] > Dmap[MERGVIEW][ind])
			{
				if (Dmap[MERGVIEW][ind] > 5)
					ODmap[MERGVIEW][ind] = Dmap[MERGVIEW][ind] - 5;
				else
					ODmap[MERGVIEW][ind] = Dmap[MERGVIEW][ind];
			}
#if USE_ZMAP
			if (OccZmap[MERGVIEW][ind] < Zmap[MERGVIEW][ind]) OccZmap[MERGVIEW][ind] = Zmap[MERGVIEW][ind];
#endif
#endif
			ind++; // Move to the next pixel
		}
}

/*
 * \brief
 *    Check if the background is on the right of the hole
 *
 * \input
 * \param xleft, y
 *    The position of the pixel that is on the left of the hole
 * \param xright, y
 *    The position of the pixel that is on the right of the hole
 * \param blksz
 *    Size of the block that is used to check depth value
 * \class member Zmap[MERGVIEW]
 *    The depth image (real depth values stroed) warped to virtual view
 * \class member Dmap[MERGVIEW]
 *    The depth image (depth levels stored) warped to virtual view
 *
 * \return
 *    1: background is on the right of the hole
 *    0: background is on the left of the hole
 */
int ViewSynthesis1D::IsBackgroundOnRight(int xleft, int xright, int y, int blksize)
{
	int i, j;
	int blksizeyMinus = 0;
	int blksizeyPlus = 0;
	int startx, starty;
	int endx, endy;
	double dleft, dright;
	int dn;

	// Calc the left side
	startx = xleft - blksize + 1;
	starty = y - blksizeyMinus;
	endx = xleft;
	endy = y + blksizeyPlus;
	if (startx < 0)
		startx = 0;
	if (starty < 0)
		starty = 0;
	if (endy >= m_height)
		endy = m_height - 1;

	dleft = 0;
	dn = 0;

	for (j = starty; j <= endy; j++)
		for (i = startx; i <= endx; i++)
		{
#if USE_ZMAP
			dleft += (double)Zmap[MERGVIEW][i + j * m_width2*m_subPelOption];
#else
			dleft += (double)Dmap[MERGVIEW][i + j * Width2*SubPelOption];
#endif
			dn++;
		}
	if (dn != 0)
		dleft = dleft / dn;

	// Calc the right side
	startx = xright;
	starty = y - blksizeyMinus;
	endx = xright + blksize - 1;
	endy = y + blksizeyPlus;

	if (endx >= m_width2 * m_subPelOption)
		endx = m_width2 * m_subPelOption - 1;
	if (starty < 0)
		starty = 0;
	if (endy >= m_height)
		endy = m_height - 1;

	dright = 0;
	dn = 0;
	for (j = starty; j <= endy; j++)
		for (i = startx; i <= endx; i++)
		{
#if USE_ZMAP
			dright += (double)Zmap[MERGVIEW][i + j * m_width2*m_subPelOption];
#else
			dright += (double)Dmap[MERGVIEW][i + j * Width2*SubPelOption];
#endif
			dn++;
		}
	if (dn != 0)
		dright = dright / dn;

	// when using Dmap as conditions

#if USE_ZMAP
	if (Znear[LEFTVIEW] > 0)
		return (dright >= dleft);
	else
		return (dright <= dleft);
#else
	return (dright <= dleft);
#endif
}
/*
 * \brief
 *    Perform hole filling
 *
 * \input
 * \param Yi
 *    The synthesized picture, current have holes. One component.
 * \class member Mask[MERGVIEW]
 *    Show the hole positions
 * \ class member Zmap[MERGVIEW]
 *    the warped depth map.
 *
 * \output
 * \param Yo
 *    The synthesized picture itself is modified due to hole filling
 *
 * \return
 *    none
 */
void ViewSynthesis1D::FillHoles(unsigned char* Yo, unsigned char* Yi)
{
	int pixelValue;
	int x, y, ind, ind2 = 0;
	int xnhole;
	int found;
	int fillDir; // 0: fail to fill; 1: fill from left; 2: fill from right

	memset(Yo, 0, m_width2*m_height*m_subPelOption);

	ind = 0;
	pixelValue = -1;
	for (y = 0; y < m_height; y++)
		for (x = 0; x < m_width2*m_subPelOption; x++)
		{
			if (Mask[MERGVIEW][ind] == HOLEPIXEL) // This is a starting pixel of a hole
			{
				if (pixelValue < 0) // This is the first pixel of a hole
				{
					found = 0;
					xnhole = x + 1;
					ind2 = ind + 1;
					while (!found && xnhole < m_width2*m_subPelOption)
					{
						if (Mask[MERGVIEW][ind2] == NOTHOLEPIXEL)
							found = 1;
						else
						{
							xnhole++; ind2++;
						}
					}

					// fill from left or from right?
					if (x == 0) // the hole is at leftmost
					{
						if (found)
							fillDir = 2;  // fill from right
						else // the whole line is a hole
							fillDir = 0;  // maybe fill hole from above
					}
					else if (found == 0) // the hole is at rightmost
					{
						fillDir = 1;  // fill from left
					}
					else if (found) // the hole is in the middle
					{
						// fill the hole using the pixel with larger depth, farther away from the camera
						if (IsBackgroundOnRight(x - 1, xnhole, y, 1))
							fillDir = 2;
						else
							fillDir = 1;
					}

					// Set the non-hole pixel
					pixelValue = 0;
					if (fillDir == 1) // from the left
						pixelValue = Yi[ind - 1];
					else if (fillDir == 2) // from the right
						pixelValue = Yi[ind2];

					Yo[ind] = pixelValue;
				}
				else
				{
					Yo[ind] = pixelValue;
					if (ind >= ind2)
						pixelValue = -1;
				}
				if (x > 0)
				{
					Zmap[MERGVIEW][ind] = Zmap[MERGVIEW][ind - 1];
#if USE_LDV
					OccZmap[MERGVIEW][ind] = OccZmap[MERGVIEW][ind - 1];
					ODmap[MERGVIEW][ind] = ODmap[MERGVIEW][ind - 1];
#endif
				}
			}
			else // This is not a hole
			{
				Yo[ind] = Yi[ind];
				pixelValue = -1;
			}

			ind++; // Move to the next pixel
		}
}

/*
 * \brief
 *    Scale down the synthesized view
 * \param Yo
 *
 * \param Yi
 *
 *
 */
void ViewSynthesis1D::ScaleDownSyn(ImageType* Yo, ImageType* Yi)
{
	if (m_subPelOption == 1)
	{
		memcpy(Yo, Yi, m_width2*m_height);
	}
	else if (m_subPelOption == 2)
	{
		ImageResample<ImageType> resample;
		resample.DownsampleView(Yo, Yi, m_width2*m_subPelOption, m_height, 2);
	}
	else if (m_subPelOption == 4)
	{
		ImageResample<ImageType> resample;
		resample.DownsampleView(Yo, Yi, m_width2*m_subPelOption, m_height, 4);
	}
	else
		printf("Error: The scale factor is not supported: %d\n", m_subPelOption);
}

/*
 * \brief
 *    Perform boundary detection based on the depth image
 *    The org function was replaced in May 2009.
 *    -Zhejiang Univ. May 2009
 *
 * \input
 * \param depth
 *    Depth map
 *
 * \output
 * \param edge
 *    Edge map: 0-NONE, 1-RISE, 2-FALL (left to right), 3-AROUND, 4-SUSPECT
 *    NONE: not boundary
 *    RISE: the top position of a rising edge from left to right
 *    FALL: the top position of a falling edge from left to right
 *    AROUND: boundary growth area around both sides of RISE or FALL edge
 *    SUSPECT: 2-pixel-wide area at left side of RISE edge and right side of FALL edge.
 *
 * \return
 *    none
 */
void ViewSynthesis1D::DetectBoundary(HoleType* edge, DepthType* depth, int width, int height, int threshold)
{
	int i, j, k;
	int ind = 0;
	int diff = 0;
	int edge_growth = cfg.getBoundaryGrowth();
	unsigned char* mask = (unsigned char*)malloc(width*height);
	memset(mask, 0, width*height);

	for (i = 0; i < height; i++)
	{
		k = i * width;
		*(edge + k) = NONE; //0th of each row
		for (j = 1; j < width; j++) //x-(x-1)
		{
			ind = k + j;
			diff = *(depth + ind) - *(depth + ind - 1);
			if (diff > threshold)
				*(edge + ind) = RISE;
			else if (diff < -threshold)
				*(edge + ind - 1) = FALL;
			else
				*(edge + ind) = NONE;
		}
	}

	for (i = 0; i < height; i++)
	{
		k = i * width;
		//to make edge at top point and 1-pixel-wide
		for (j = 1; j < width - 1; j++)
		{
			ind = k + j;
			if (edge[ind] == RISE && edge[ind + 1] == NONE && depth[ind + 1] - depth[ind] > 5)       mask[ind + 1] = 50; // later be changed to RISE
			else if (edge[ind] == FALL && edge[ind - 1] == NONE && depth[ind - 1] - depth[ind] > 5)  mask[ind - 1] = 60; // later be changed to FALL
			else if (edge[ind] == RISE && edge[ind + 1] == RISE)                               mask[ind] = 40;
			else if (edge[ind] == FALL && edge[ind - 1] == FALL)                               mask[ind] = 40;   // later be changed to NONE
		}

		for (j = 0; j < width; j++)
		{
			ind = k + j;
			if (mask[ind] == 50)
			{
				edge[ind] = RISE;  edge[ind - 1] = NONE;
			}
			else if (mask[ind] == 60)
			{
				edge[ind] = FALL;  edge[ind + 1] = NONE;
			}
			else if (mask[ind] == 40)
			{
				edge[ind] = NONE;
			}
		}

		//to mark 2-pixel-wide area at left side of RISE edge and at right side of FALL edge
		for (j = 2; j < width - 2; j++)
		{
			ind = k + j;
			if (edge[ind + 1] == RISE || edge[ind + 2] == RISE)
				edge[ind] = SUSPECT;
			else if (edge[ind - 1] == FALL || edge[ind - 2] == FALL)
				edge[ind] = SUSPECT;
		}

		//to mark edge_growth-pixel-wide area around strong depth edge (both sides)
		for (j = edge_growth; j < width - edge_growth; j++)
		{
			ind = k + j;
			//if current pixel is FALL, mask the right-40 pixel as boundary pixel
			if (edge[ind] == FALL || edge[ind] == RISE)
				for (int p = ind - edge_growth; p <= ind + edge_growth; p++)
					if (edge[p] == NONE)
						edge[p] = AROUND;
		}
	}

#if DEBUG_ZHEJIANG
	//debug
	int count1 = 0;
	int count2 = 0;
	int sz = width * height;
	ind = 0;
	for (i = 0; i < sz; i++)
	{
		if (edge[ind] == RISE) count1++;
		else if (edge[ind] == FALL) count2++;
		ind++;
	}
	fprintf(stderr, "\n Debug Info: Depth edge pixels: RISE = %d; FALL =%d ", count1, count2);
#endif

	free(mask);
}

/*
 * \brief
 *    Check a small block around a specified pixel if there is any boundary pixel
 *
 * \input
 * \param BoundaryArray
 *    A map indicating which pixels are regarded as boundaries. (the mask has the size of Width2*Height)
 * \param x, y
 *    The pixel position
 *
 * \return
 *    true: if there is any boundary pixel around
 *    false: if there is no any boundary pixel around
 */
bool ViewSynthesis1D::IsBoundary(unsigned char* BoundaryArray, int x, int y)
{
	if (BoundaryArray[y*m_width2 + x] != NONE) //which means == RISE || FALL || AROUND
		return true;
	else
		return false;
}

/*
 * \brief
 *    Export the hole positions out of the lib
 * \param SynMask
 *    Write the hole mask into this array
 * \return
 *    0
 */
int  ViewSynthesis1D::GetSynMask(unsigned char* SynMask)
{
	int x, y;
	//int i, j;
	//int flag;

	// Mask
	if (SynMask)
	{
		if (m_subPelOption == 1 && m_upsampleRefs == 1)
		{
			memcpy(SynMask, Mask[MERGVIEW], m_width*m_height);
		}
		else
		{
			for (y = 0; y < m_height; y++)
				for (x = 0; x < m_width; x++)
				{
					// ********
					// Depending how you want to look at the mask, you can change the following code:
					// ********

					//flag = HOLEPIXEL;
					//for (i = - SubPelOption*UpsampleRefs; i <= SubPelOption*UpsampleRefs; i++)
					//{
					//  j = x*SubPelOption*UpsampleRefs + i;
					//  if (j >= 0 && j < Width*SubPelOption*UpsampleRefs)
					//    flag = (flag && Mask[MERGVIEW][j + y*Width*SubPelOption*UpsampleRefs]);
					//}
					//SynMask[x+y*Width] = flag;

					SynMask[x + y * m_width] = Mask[MERGVIEW][x*m_subPelOption*m_upsampleRefs + y * m_width*m_subPelOption*m_upsampleRefs];
				}
		}
	}

	return 0;
}

/*
 * \brief
 *    Get the synthesized depth image. Note that we simply copy the int pixel for output.
 *
 * \param SynDepth
 *    The destination where to write the depth image
 *
 * \return
 *    0
 */
int  ViewSynthesis1D::GetSynDepth(unsigned char* SynDepth)
{
	int i, x, y;

	// Depth
	if (SynDepth)
	{
		if (m_subPelOption == 1 && m_upsampleRefs == 1)
		{
			for (i = 0; i < m_width*m_height; i++)
				SynDepth[i] = Dmap[MERGVIEW][i];
		}
		else
		{
			for (y = 0; y < m_height; y++)
				for (x = 0; x < m_width; x++)
				{
					// In case of subpel precision, the following code simply pick up the integer pixels.
					SynDepth[x + y * m_width] = Dmap[MERGVIEW][x*m_subPelOption*m_upsampleRefs + y * m_width*m_subPelOption*m_upsampleRefs];
				}
		}
	}

	return 0;
}

/*
 * \brief
 *    Perform view interpolation using two refence views
 *
 * \input
 *    The images are arranged in a one dimension array
 * \param RefLeft
 *    The left ref view image. YUV 444
 * \param RefRight
 *    The right ref view image YUV 444
 * \param RefDepthLeft
 *    The left ref depth view image
 * \param RefDepthRight
 *    The right ref depth view image
 *
 *
 * \output
 * \param Syn
 *    The synthesized view. YUV 420
 * \param SynMask
 *    The mask showing the hole positions
 *
 * \return
 *    true: success
 *    false: fail
 */
bool ViewSynthesis1D::apply(unique_ptr<Image<ImageType>>& imgSyn)
{
	upsample();

	SetFrameNumber(getFrameNumber()); //Zhejiang

	ImageType* RefLeft = m_views[0]->getImage()->getBuffer1D();
	ImageType* RefRight = m_views[1]->getImage()->getBuffer1D();
	DepthType* RefDepthLeft = m_views[0]->getDepth()->getBuffer1D();
	DepthType* RefDepthRight = m_views[1]->getDepth()->getBuffer1D();

	ImageType* Syn = imgSyn->getBuffer1D();

	ImageType* pRefLeft;
	ImageType* pRefRight;
	DepthType* pRefDepthLeft;
	DepthType* pRefDepthRight;
	ImageResample<ImageType> Resampling;
	ImageResample<DepthType> ResamplingDepth;

	m_width2 = m_width * m_upsampleRefs;

	//Zhejiang, Temporal Improvement Option
	if (cfg.getTemporalImprovementOption() == 1)
	{
		TemporalImprovementMethod(RefLeft, RefViewLast[0], RefDepthLeft, RefDepthLast[0], FrameNumber, m_width, m_height);
		TemporalImprovementMethod(RefRight, RefViewLast[1], RefDepthRight, RefDepthLast[1], FrameNumber, m_width, m_height);
	}

	// Upsample ref views, if necessary
	if (m_upsampleRefs == 1)
	{
		pRefLeft = RefView[LEFTVIEW] = RefLeft;
		pRefRight = RefView[RGHTVIEW] = RefRight;
		pRefDepthLeft = RefDepth[LEFTVIEW] = RefDepthLeft;
		pRefDepthRight = RefDepth[RGHTVIEW] = RefDepthRight;
	}
	else {
		pRefLeft = RefView[LEFTVIEW];
		pRefRight = RefView[RGHTVIEW];
		pRefDepthLeft = RefDepth[LEFTVIEW];
		pRefDepthRight = RefDepth[RGHTVIEW];
		Resampling.UpsampleView(pRefLeft, RefLeft, m_width, m_height, m_upsampleRefs);
		Resampling.UpsampleView(&pRefLeft[m_width2*m_height], &RefLeft[m_width*m_height], m_width, m_height, m_upsampleRefs);
		Resampling.UpsampleView(&pRefLeft[m_width2*m_height * 2], &RefLeft[m_width*m_height * 2], m_width, m_height, m_upsampleRefs);
		Resampling.UpsampleView(pRefRight, RefRight, m_width, m_height, m_upsampleRefs);
		Resampling.UpsampleView(&pRefRight[m_width2*m_height], &RefRight[m_width*m_height], m_width, m_height, m_upsampleRefs);
		Resampling.UpsampleView(&pRefRight[m_width2*m_height * 2], &RefRight[m_width*m_height * 2], m_width, m_height, m_upsampleRefs);
		ResamplingDepth.UpsampleView(pRefDepthLeft, RefDepthLeft, m_width, m_height, m_upsampleRefs);
		ResamplingDepth.UpsampleView(pRefDepthRight, RefDepthRight, m_width, m_height, m_upsampleRefs);
	}

	// Warp the ref pictures
	ForwardWarp(); // pRefLeft, pRefRight, pRefDepthLeft, pRefDepthRight

	// Merge the two synthesized view into one
	Merge();

	// Fill holes
	FillHoles(SynY[HLFLVIEW], SynY[MERGVIEW]);
	FillHoles(SynU[HLFLVIEW], SynU[MERGVIEW]);
	FillHoles(SynV[HLFLVIEW], SynV[MERGVIEW]);
	// Prepare the output
	if (Syn)
	{
		// Scale down the output
		ScaleDownSyn(SynY[FINLVIEW], SynY[HLFLVIEW]);
		ScaleDownSyn(SynU[FINLVIEW], SynU[HLFLVIEW]);
		ScaleDownSyn(SynV[FINLVIEW], SynV[HLFLVIEW]);

		if (m_upsampleRefs != 1)
		{
			Resampling.DownsampleView(SynY[FINLVIEW], SynY[FINLVIEW], m_width2, m_height, m_upsampleRefs);
			Resampling.DownsampleView(SynU[FINLVIEW], SynU[FINLVIEW], m_width2, m_height, m_upsampleRefs);
			Resampling.DownsampleView(SynV[FINLVIEW], SynV[FINLVIEW], m_width2, m_height, m_upsampleRefs);
		}

		// Y
		memcpy(Syn, SynY[FINLVIEW], m_width*m_height);
		// Downsample the UV
		Resampling.PictureResample444to420(&Syn[m_width*m_height], SynU[FINLVIEW], m_width / 2, m_height / 2);
		Resampling.PictureResample444to420(&Syn[m_width*m_height * 5 / 4], SynV[FINLVIEW], m_width / 2, m_height / 2);
	}

	// fill the Image buffer 1D for BNR
	ImageType* tar_L, *tar_R;

	// Left Synthesized Image
	tar_L = m_imgLeftWithHole->getBuffer1D();
	memcpy(&tar_L[m_height*m_width * 0], GetSynColorLeftY(), m_height*m_width);
	memcpy(&tar_L[m_height*m_width * 1], GetSynColorLeftU(), m_height*m_width);
	memcpy(&tar_L[m_height*m_width * 2], GetSynColorLeftV(), m_height*m_width);

	// Right Synthesized Image
	tar_R = m_imgRightWithHole->getBuffer1D();
	memcpy(&tar_R[m_height*m_width * 0], GetSynColorRightY(), m_height*m_width);
	memcpy(&tar_R[m_height*m_width * 1], GetSynColorRightU(), m_height*m_width);
	memcpy(&tar_R[m_height*m_width * 2], GetSynColorRightV(), m_height*m_width);

	// Left Synthesized Depth Image
	memcpy(m_depthLeftWithHole->getBuffer1D(), GetSynDepthLeft(), m_height*m_width);

	// Right Synthesized Depth Image
	memcpy(m_depthRightWithHole->getBuffer1D(), GetSynDepthRight(), m_height*m_width);

	if (cfg.getBoundaryNoiseRemoval() == cfg.BNR_ENABLED) {
		BoundaryNoiseRemoval* boundaryNoiseRemoval = new BoundaryNoiseRemoval1D();
		boundaryNoiseRemoval->SetduPrincipal(GetduPrincipal());
		boundaryNoiseRemoval->SetLTranslationLeft(GetLTranslation());
		boundaryNoiseRemoval->SetZfar(GetZfar());
		boundaryNoiseRemoval->SetZnear(GetZnear());

		if (!boundaryNoiseRemoval->apply(GetSynLeftWithHole(), GetSynRightWithHole(), GetSynDepthLeftWithHole(), GetSynDepthRightWithHole(), GetSynHoleLeft(), GetSynHoleRight(), imgSyn, true/*1D Mode*/))
			return false;
	}

	return true;
}

//Zhejiang
//FindDepthMaxMin -- search less point to get the approximate Zmax and Zmin
void ViewSynthesis1D::FindDepthMaxMin(DepthType* map, int ViewID)
{
	int i, j, k, ind;
	double z;
	Maxdk = Mindk = 0;
	Maxz = 0;
	Minz = MAX_DEPTH - 1;

	for (i = 0; i < m_height; i = i + 16)
	{
		k = i * m_width;
		for (j = 0; j < m_width; j = j + 16)
		{
			ind = k + j;
			if (map[ind] > Maxz)		Maxz = map[ind];
			else if (map[ind] < Minz)   Minz = map[ind];
		}
	}

	z = (double)1.0 / ((Maxz / (MaxTypeValue<DepthType>() - 1.0)) * (1.0 / Znear[ViewID] - 1.0 / Zfar[ViewID]) + (1.0 / Zfar[ViewID]));
	Maxdk = (int)(cfg.getFocalLength() * LTranslation[ViewID] / z - abs(duPrincipal[ViewID]) + 1); //ceiling
	z = (double)1.0 / ((Minz / (MaxTypeValue<DepthType>() - 1.0)) * (1.0 / Znear[ViewID] - 1.0 / Zfar[ViewID]) + (1.0 / Zfar[ViewID]));
	Mindk = (int)(cfg.getFocalLength() * LTranslation[ViewID] / z - abs(duPrincipal[ViewID]) + 0); //flooring

	//UpsampleRefs, SubPelOption
	Maxdk = Maxdk * m_upsampleRefs;
	Mindk = Mindk * m_upsampleRefs;

	SplattingDepthThreshold = (int)((Maxz - Minz) *0.6 + Minz);
}

//2009.05.04, improvement, Zhejiang University(a little different from m16041: Temporal Improvement Method in View Synthesis)
//-if a block is moving, then its neigbouring 4*4 blocks are also dertermined to be moving,
//--since the object boundary on depth map is not restrict to depth boundary.
void ViewSynthesis1D::TemporalImprovementMethod(ImageType *ImageCur, ImageType *ImageLast, DepthType *DepthCur, DepthType *DepthLast,
	int frame, int Width, int Height)
{
	int SAD_TH = 100;     //threshold of sad in 4*4 block motion detection
	double weight = 0.75;  //weight value for depth map weighting equation

	//internal variables
	int diff = 0;
	int ind = 0;
	int s = 0;
	unsigned char *flag = NULL;
	int t = 0;

	flag = (unsigned char*)malloc(Width / 4);

	if (0 == frame) // first frame need not the weighting, but need to prepare the data
	{
		memcpy(ImageLast, ImageCur, Width*Height);
		memcpy(DepthLast, DepthCur, Width*Height);
	}
	else
	{
		for (int y = 0; y < Height / 4; y++)
		{
			//motion detection by SSE -- do a line
			for (int x = 0; x < Width / 4; x++)
			{
				ind = y * Width * 4 + x * 4;
				s = 0;
				for (int j = 0; j < 4; j++)
					for (int i = 0; i < 4; i++)
					{
						diff = (ImageCur[ind + j * Width + i] - ImageLast[ind + j * Width + i]);
						s += abs(diff);	 //SAD
					}
				if (s < SAD_TH) flag[x] = 0;
				else         flag[x] = 1;
			}
			//temporal weighting according to motion detection result -- do a line
			for (int x = 0; x < Width / 4; x++)
			{
				ind = y * Width * 4 + x * 4;

#if TEMPOBLOCK3
				//3 block
				if (x == 0)					t = flag[x] + flag[x + 1];
				else if (x == Width / 4 - 1)   t = flag[x - 1] + flag[x];
				else						t = flag[x - 1] + flag[x] + flag[x + 1];
#else
				//5 block
				if (x == 0)					t = flag[x] + flag[x + 1] + flag[x + 2];
				else if (x == 1)			t = flag[x - 1] + flag[x] + flag[x + 1] + flag[x + 2];
				else if (x == Width / 4 - 2)    t = flag[x - 2] + flag[x - 1] + flag[x] + flag[x + 1];
				else if (x == Width / 4 - 1)    t = flag[x - 2] + flag[x - 1] + flag[x];
				else						t = flag[x - 2] + flag[x - 1] + flag[x] + flag[x + 1] + flag[x + 2];
#endif

				if (t == 0)// if not moving
					for (int j = 0; j < 4; j++)
						for (int i = 0; i < 4; i++)
							DepthCur[ind + j * Width + i] = (unsigned char)(DepthLast[ind + j * Width + i] * weight + DepthCur[ind + j * Width + i] * (1 - weight));
				//else, do nothing
			}
		}

		memcpy(ImageLast, ImageCur, Width*Height);
		memcpy(DepthLast, DepthCur, Width*Height);
	}
}