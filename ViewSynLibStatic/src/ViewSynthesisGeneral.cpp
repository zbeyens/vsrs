#include "ViewSynthesis.h"


ViewSynthesisGeneral::ViewSynthesisGeneral()
	: cfg(ConfigSyn::getInstance())
{
	m_imgSynLeftforBNR = NULL;
	m_imgSynRightforBNR = NULL;
	m_imgDepthLeftforBNR = NULL;
	m_imgDepthRightforBNR = NULL;
	m_imgHoleLeftforBNR = NULL;
	m_imgHoleRightforBNR = NULL;
}

ViewSynthesisGeneral::~ViewSynthesisGeneral()
{
	xReleaseMemory();
}

void ViewSynthesisGeneral::xReleaseMemory()
{
	Tools::safeDelete(m_pcViewSynthesisLeft);
	Tools::safeDelete(m_pcViewSynthesisRight);

	Tools::safeReleaseImage(m_imgSynLeftforBNR);
	Tools::safeReleaseImage(m_imgSynRightforBNR);
	Tools::safeReleaseImage(m_imgDepthLeftforBNR);
	Tools::safeReleaseImage(m_imgDepthRightforBNR);
	Tools::safeReleaseImage(m_imgHoleLeftforBNR);
	Tools::safeReleaseImage(m_imgHoleRightforBNR);
}

bool ViewSynthesisGeneral::InitLR()
{
	m_pcViewSynthesisLeft = new Sample();
	m_pcViewSynthesisRight = new Sample();

	if (!m_pcViewSynthesisLeft->Init(0))  return false;
	if (!m_pcViewSynthesisRight->Init(1))  return false;

	computeWeightLR();

	initResultImages();

	return true;
}

void ViewSynthesisGeneral::computeWeightLR()
{
	m_dWeightLeft = m_pcViewSynthesisRight->getCam().getBaselineDistance();
	m_dWeightRight = m_pcViewSynthesisLeft->getCam().getBaselineDistance();
	double dTotalBaseline = m_dWeightLeft + m_dWeightRight;
	m_dWeightLeft /= dTotalBaseline;
	m_dWeightRight /= dTotalBaseline;
}

void ViewSynthesisGeneral::initResultImages()
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	m_imgBlended.create(sw, sh, IMAGE_CHANNELS);
	m_imgBlendedDepth.create(sw, sh, BLENDED_DEPTH_CHANNELS); // NICT
	m_imgInterpolatedView.create(sw, sh, IMAGE_CHANNELS);
	m_imgMask[0].create(sw, sh, MASK_CHANNELS);
	m_imgMask[1].create(sw, sh, MASK_CHANNELS);
	m_imgMask[2].create(sw, sh, MASK_CHANNELS);
}

int ViewSynthesisGeneral::DoOneFrameGeneral(ImageType*** RefLeft, ImageType*** RefRight, DepthType** RefDepthLeft, DepthType** RefDepthRight, CIYuv<ImageType> *pSynYuvBuffer)
{
	ImageType*** pRefLeft = RefLeft;
	ImageType*** pRefRight = RefRight;
	DepthType** pRefDepthLeft = RefDepthLeft;
	DepthType** pRefDepthRight = RefDepthRight;

	if (!m_pcViewSynthesisLeft->xSynthesizeView(pRefLeft, pRefDepthLeft))  return false;
	if (!m_pcViewSynthesisRight->xSynthesizeView(pRefRight, pRefDepthRight)) return false;

	// GIST added
	if (m_imgSynLeftforBNR == NULL) { m_imgSynLeftforBNR = cvCreateImage(cvSize(cfg.getSourceWidth(), cfg.getSourceHeight()), sizeof(ImageType) * 8, 3); }
	if (m_imgSynRightforBNR == NULL) { m_imgSynRightforBNR = cvCreateImage(cvSize(cfg.getSourceWidth(), cfg.getSourceHeight()), sizeof(ImageType) * 8, 3); }
	if (m_imgDepthLeftforBNR == NULL) { m_imgDepthLeftforBNR = cvCreateImage(cvSize(cfg.getSourceWidth(), cfg.getSourceHeight()), sizeof(DepthType) * 8, 1); }
	if (m_imgDepthRightforBNR == NULL) { m_imgDepthRightforBNR = cvCreateImage(cvSize(cfg.getSourceWidth(), cfg.getSourceHeight()), sizeof(DepthType) * 8, 1); }
	if (m_imgHoleLeftforBNR == NULL) { m_imgHoleLeftforBNR = cvCreateImage(cvSize(cfg.getSourceWidth(), cfg.getSourceHeight()), sizeof(HoleType) * 8, 1); }
	if (m_imgHoleRightforBNR == NULL) { m_imgHoleRightforBNR = cvCreateImage(cvSize(cfg.getSourceWidth(), cfg.getSourceHeight()), sizeof(HoleType) * 8, 1); }
	cvCopy(m_pcViewSynthesisLeft->getVirtualImage(), m_imgSynLeftforBNR);
	cvCopy(m_pcViewSynthesisRight->getVirtualImage(), m_imgSynRightforBNR);
	cvCopy(m_pcViewSynthesisLeft->getVirtualDepthMap(), m_imgDepthLeftforBNR);
	cvCopy(m_pcViewSynthesisRight->getVirtualDepthMap(), m_imgDepthRightforBNR);
	cvCopy(m_pcViewSynthesisLeft->getHolePixels(), m_imgHoleLeftforBNR);
	cvCopy(m_pcViewSynthesisRight->getHolePixels(), m_imgHoleRightforBNR);
	// GIST end

	// pixels which will be replaced by pixels synthesized from right view
	if (cfg.getIvsrsInpaint() == 1)
	{
		cvAnd(m_pcViewSynthesisLeft->getHolePixels(), m_pcViewSynthesisRight->getSynthesizedPixels(), m_imgMask[1].getImageIpl()); // NICT use same hole mask
	}
	else
	{
		cvAnd(m_pcViewSynthesisLeft->getUnstablePixels(), m_pcViewSynthesisRight->getSynthesizedPixels(), m_imgMask[1].getImageIpl()); // Left dilated Mask[0] * Right Success -> Left Mask[3] // dilated hole fillable by Right
	}

	if (cfg.getViewBlending() == 1)
	{
		if (m_dWeightLeft >= m_dWeightRight)  // if closer to Left
		{
			cvCopy(m_pcViewSynthesisRight->getVirtualImage(), m_pcViewSynthesisLeft->getVirtualImage(), m_imgMask[1].getImageIpl());  // Right * Mask[3] -> Left // dilated hole may left
		}
		else                               // if closer to Right
		{
			cvCopy(m_pcViewSynthesisRight->getVirtualImage(), m_pcViewSynthesisLeft->getVirtualImage(), m_pcViewSynthesisRight->getSynthesizedPixels()); // Right VirtualImage * success -> Left VirtualImage
		}
	}
	else {
		cvCopy(m_pcViewSynthesisRight->getVirtualImage(), m_pcViewSynthesisLeft->getVirtualImage(), m_imgMask[1].getImageIpl()); // Right VirtualImage * hole fillabl by Right -> Left VirtualImage // Left hole is filled by Right
		if (cfg.getIvsrsInpaint() == 1)
		{
			cvCopy(m_pcViewSynthesisRight->getVirtualDepthMap(), m_pcViewSynthesisLeft->getVirtualDepthMap(), m_imgMask[1].getImageIpl()); // Right VirtualDepth * hole fillabl by Right -> Left VirtualDepth //NICT
		}
	}

	// pixels which will be replaced by pixels synthesized from left view
	if (cfg.getIvsrsInpaint() == 1)
	{
		cvAnd(m_pcViewSynthesisRight->getHolePixels(), m_pcViewSynthesisLeft->getSynthesizedPixels(), m_imgMask[2].getImageIpl()); // NICT use same hole mask
	}
	else
	{
		cvAnd(m_pcViewSynthesisRight->getUnstablePixels(), m_pcViewSynthesisLeft->getSynthesizedPixels(), m_imgMask[2].getImageIpl()); // Right dilated Mask[0] * Left Success -> Mask[4] // dilated hole fillable by Left
	}

	if (cfg.getViewBlending() == 1)
	{
		if (m_dWeightLeft <= m_dWeightRight) // if closer to Right
		{
			cvCopy(m_pcViewSynthesisLeft->getVirtualImage(), m_pcViewSynthesisRight->getVirtualImage(), m_imgMask[2].getImageIpl()); // Left VirtualImage * Mask[4] -> Right VirtualImage
		}
		else                              // if close to Left
		{
			cvCopy(m_pcViewSynthesisLeft->getVirtualImage(), m_pcViewSynthesisRight->getVirtualImage(), m_pcViewSynthesisLeft->getSynthesizedPixels()); // Left VirtualImage * success -> Right VirtualImage
		}
	}
	else {
		cvCopy(m_pcViewSynthesisLeft->getVirtualImage(), m_pcViewSynthesisRight->getVirtualImage(), m_imgMask[2].getImageIpl()); // Left VirtualImage * hole fillable by Left ->Right VirtualImage
		if (cfg.getIvsrsInpaint() == 1)
		{
			cvCopy(m_pcViewSynthesisLeft->getVirtualDepthMap(), m_pcViewSynthesisRight->getVirtualDepthMap(), m_imgMask[2].getImageIpl()); // Left VirtualDedpth * hole fillable by Left ->Right VirtualDepth // NICT
		}
	}

	// pixels which couldn't be synthesized from both left and right -> inpainting
	cvAnd(m_pcViewSynthesisLeft->getHolePixels(), m_pcViewSynthesisRight->getHolePixels(), m_imgMask[0].getImageIpl()); // Left Hole * Right Hole -> Mask[2] // common hole,

#define GETUCHAR(x,ptr) (((unsigned char*)x)[ptr])
	IplImage *DepthLeft = m_pcViewSynthesisLeft->getVirtualDepthMap();
	IplImage *DepthRight = m_pcViewSynthesisRight->getVirtualDepthMap();
	IplImage *ImageLeft = m_pcViewSynthesisLeft->getVirtualImage();
	IplImage *ImageRight = m_pcViewSynthesisRight->getVirtualImage();
	IplImage *SynthesizedLeft = m_pcViewSynthesisLeft->getHolePixels();
	IplImage *SynthesizedRight = m_pcViewSynthesisRight->getHolePixels();
	for (int h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (int w = 0; w < cfg.getSourceWidth(); w++)
		{
			int ptv = w + h * cfg.getSourceWidth();
			m_imgBlended.getImageIpl()->imageData[ptv * 3 + 0] = 0;
			m_imgBlended.getImageIpl()->imageData[ptv * 3 + 1] = 0;
			m_imgBlended.getImageIpl()->imageData[ptv * 3 + 2] = 0;
			if (m_imgMask[0].getImageIpl()->imageData[ptv] != 0) continue;

			if ((abs(((DepthType*)DepthLeft->imageData[ptv]) - ((DepthType*)DepthRight->imageData[ptv])) < cfg.getDepthBlendDiff())) // left and right are close to each other (NICT)
			{
				((ImageType*)m_imgBlended.getImageIpl()->imageData)[ptv * 3 + 0] = CLIP3((((ImageType*)ImageLeft->imageData)[ptv * 3 + 0] * m_dWeightLeft + ((ImageType*)ImageRight->imageData)[ptv * 3 + 0] * m_dWeightRight) / (m_dWeightLeft + m_dWeightRight), 0, MAX_LUMA - 1);
				((ImageType*)m_imgBlended.getImageIpl()->imageData)[ptv * 3 + 1] = CLIP3((((ImageType*)ImageLeft->imageData)[ptv * 3 + 1] * m_dWeightLeft + ((ImageType*)ImageRight->imageData)[ptv * 3 + 1] * m_dWeightRight) / (m_dWeightLeft + m_dWeightRight), 0, MAX_LUMA - 1);
				((ImageType*)m_imgBlended.getImageIpl()->imageData)[ptv * 3 + 2] = CLIP3((((ImageType*)ImageLeft->imageData)[ptv * 3 + 2] * m_dWeightLeft + ((ImageType*)ImageRight->imageData)[ptv * 3 + 2] * m_dWeightRight) / (m_dWeightLeft + m_dWeightRight), 0, MAX_LUMA - 1);

				if (cfg.getIvsrsInpaint() == 1)
				{
					((DepthType*)m_imgBlendedDepth.getImageIpl()->imageData)[ptv] = CLIP3((((DepthType*)DepthLeft->imageData)[ptv] * m_dWeightLeft + ((DepthType*)DepthRight->imageData)[ptv] * m_dWeightRight) / (m_dWeightLeft + m_dWeightRight), 0, MAX_DEPTH - 1);
				}
			}
			else if ((((DepthType*)DepthLeft->imageData[ptv]) > ((DepthType*)DepthRight->imageData[ptv]))) //Fix to compare z // left is nearer (NICT)
			{
				m_imgBlended.getImageIpl()->imageData[ptv * 3 + 0] = ImageLeft->imageData[ptv * 3 + 0];
				m_imgBlended.getImageIpl()->imageData[ptv * 3 + 1] = ImageLeft->imageData[ptv * 3 + 1];
				m_imgBlended.getImageIpl()->imageData[ptv * 3 + 2] = ImageLeft->imageData[ptv * 3 + 2];

				if (cfg.getIvsrsInpaint() == 1)
				{
					m_imgBlendedDepth.getImageIpl()->imageData[ptv] = DepthLeft->imageData[ptv];
				}
			}
			else /*if((m_imgMask[1].getImageIpl()->imageData[ptv]!=0))*/ //Fix should be mixed together // Right is closer
			{
				m_imgBlended.getImageIpl()->imageData[ptv * 3 + 0] = ImageRight->imageData[ptv * 3 + 0];
				m_imgBlended.getImageIpl()->imageData[ptv * 3 + 1] = ImageRight->imageData[ptv * 3 + 1];
				m_imgBlended.getImageIpl()->imageData[ptv * 3 + 2] = ImageRight->imageData[ptv * 3 + 2];

				if (cfg.getIvsrsInpaint() == 1)
				{
					m_imgBlendedDepth.getImageIpl()->imageData[ptv] = DepthRight->imageData[ptv];
				}
			}
		}
	}

	//#else
	//cvAddWeighted(m_pcViewSynthesisLeft->getVirtualImage(), m_dWeightLeft, m_pcViewSynthesisRight->getVirtualImage(), m_dWeightRight, 0, m_imgBlended.getImageIpl()); // Left VImage * LWeight + Rigt VImage * RWeight -> Blended
	//#endif

	if (cfg.getIvsrsInpaint() == 1)
	{
		int hptv, ptv, var;
		bool holeflag, inpaintflag, lflag, mflag, rflag, filterflag;
		bool llflag, lmflag, mrflag, rrflag;
		int leftptv, rightptv, delta, midptv, lptv, mptv, rptv, refptv, varptv;
		int llptv, lmptv, mrptv, rrptv;
		int ref;
		int refdepth;
		int v, leftw, rightw, middle, left, right;

		for (int h = 0; h < cfg.getSourceHeight(); h++)
		{
			holeflag = false;
			inpaintflag = false;
			hptv = h * cfg.getSourceWidth();
			for (int w = 0; w < cfg.getSourceWidth(); w++)
			{
				ptv = w + hptv;
				if (m_imgMask[0].getImageIpl()->imageData[ptv] != 0) // hole
				{
					if (w == 0) // hole start at 0
					{
						holeflag = true;
						leftw = w - 1;
						leftptv = ptv - 1;
						refdepth = MAX_DEPTH - 1; // set left depth(255) to ref depth
					}
					else if (w == cfg.getSourceWidth() - 1) // hole end at W
					{
						inpaintflag = true;
						rightw = w + 1;
						rightptv = ptv + 1;
						// check hole start
						if (holeflag == false) // 1 pel hole
						{
							leftw = w - 1;
							leftptv = ptv - 1;
							refptv = leftptv;
							refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv]; // set left depth to ref depth
						}// else, left was set before
					}
					else if (holeflag == false) // hole start at middle
					{
						holeflag = true;
						leftw = w - 1;
						leftptv = ptv - 1;
						refptv = leftptv;
						refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv]; // set left depth to ref depth
					} // else, middle in hole, do nothing
				} // hole end
				else // Mask[2] = 0 not hole
				{
					if (holeflag == true) // hole end
					{
						holeflag = false;
						inpaintflag = true;
						rightw = w;
						rightptv = ptv;
						if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[rightptv]) // set right depth to ref depth
						{
							refptv = rightptv;
							refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
						}
					} // else, middle in not hole, do nothing
				} // not hole end

				// inpaint
				if (inpaintflag == true)
				{
					inpaintflag = false;
					// search top ref
					middle = (leftw + rightw) >> 1;
					left = middle;
					right = cfg.getSourceWidth() - 1 - middle;
					midptv = (leftptv + rightptv) >> 1;
					mptv = midptv;

					lflag = mflag = rflag = false;
					llflag = lmflag = mrflag = rrflag = false;

					if (h < cfg.getSourceHeight() * 2 / 3) // upper half, search sky
					{
						for (v = 1; v <= h; v++) // sesarch top
						{
							mptv -= cfg.getSourceWidth();
							if (mflag == false && m_imgMask[0].getImageIpl()->imageData[mptv] == 0) // not hole
							{
								mflag = true;
								if (refdepth > (DepthType) m_imgBlendedDepth.getImageIpl()->imageData[mptv])
								{
									refptv = mptv;
									refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
								}
							} // else hole, do nothing

							delta = v << 1;
							if (delta > left) llflag = true;
							llptv = mptv - delta;
							if (llflag == false && m_imgMask[0].getImageIpl()->imageData[llptv] == 0) // not hole
							{
								llflag = true;
								if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[llptv])
								{
									refptv = llptv;
									refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
								}
							} // else hole, do nopthing

							delta = v;
							if (delta > left) lflag = true;
							lptv = mptv - delta;
							if (lflag == false && m_imgMask[0].getImageIpl()->imageData[lptv] == 0) // not hole
							{
								lflag = true;
								if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[lptv])
								{
									refptv = lptv;
									refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
								}
							} // else hole, do nopthing

							delta = v >> 1;
							if (delta > left) lmflag = true;
							lmptv = mptv - delta;
							if (lmflag == false && m_imgMask[0].getImageIpl()->imageData[lmptv] == 0) // not hole
							{
								lmflag = true;
								if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[lmptv])
								{
									refptv = lmptv;
									refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
								}
							} // else hole, do nopthing

							delta = v >> 1;
							if (delta > right) mrflag = true;
							mrptv = mptv + delta;
							if (mrflag == false && m_imgMask[0].getImageIpl()->imageData[mrptv] == 0) // not hole
							{
								mrflag = true;
								if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[mrptv])
								{
									refptv = mrptv;
									refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
								}
							} // else hole, do nothing

							delta = v;

							if (delta > right) rflag = true;
							rptv = mptv + delta;
							if (rflag == false && m_imgMask[0].getImageIpl()->imageData[rptv] == 0) // not hole
							{
								rflag = true;
								if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[rptv])
								{
									refptv = rptv;
									refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
								}
							} // else hole, do nothing

							delta = v << 1;
							if (delta > right) rrflag = true;
							rrptv = mptv + delta;
							if (rrflag == false && m_imgMask[0].getImageIpl()->imageData[rrptv] == 0) // not hole
							{
								rrflag = true;
								if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[rrptv])
								{
									refptv = rrptv;
									refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
								}
							} // else hole, do nothing

							if (llflag == true && lflag == true && lmflag == true && mflag == true && mrflag == true && rflag == true && rrflag == true) break; // stop search
						} // v end
					} // else lower half, do followings

					  // check bottom half, search farthest object
					mptv = midptv;
					lflag = mflag = rflag = false;
					llflag = lmflag = mrflag = rrflag = false;

					for (v = 1; v < cfg.getSourceHeight() - h; v++) // sesarch bottom
					{
						mptv += cfg.getSourceWidth();
						if (mflag == false && m_imgMask[0].getImageIpl()->imageData[mptv] == 0) // not hole
						{
							mflag = true;
							if (refdepth > (DepthType) m_imgBlendedDepth.getImageIpl()->imageData[mptv])
							{
								refptv = mptv;
								refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
							}
						}

						delta = v << 1;
						if (delta > left) llflag = true;
						llptv = mptv - delta;

						if (llflag == false && m_imgMask[0].getImageIpl()->imageData[llptv] == 0) // not hole
						{
							llflag = true;
							if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[llptv])
							{
								refptv = llptv;
								refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
							}
						}

						delta = v;
						if (delta > left) lflag = true;
						lptv = mptv - delta;
						if (lflag == false && m_imgMask[0].getImageIpl()->imageData[lptv] == 0) // not hole
						{
							lflag = true;
							if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[lptv])
							{
								refptv = lptv;
								refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
							}
						}

						delta = v >> 1;
						if (delta > left) lmflag = true;
						lmptv = mptv - delta;
						if (lmflag == false && m_imgMask[0].getImageIpl()->imageData[lmptv] == 0) // not hole
						{
							lmflag = true;
							if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[lmptv])
							{
								refptv = lmptv;
								refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
							}
						}

						delta = v >> 1;
						if (delta > right) mrflag = true;
						mrptv = mptv + delta;
						if (mrflag == false && m_imgMask[0].getImageIpl()->imageData[mrptv] == 0) // not hole
						{
							mrflag = true;
							if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[mrptv])
							{
								refptv = mrptv;
								refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
							}
						}

						delta = v;
						if (delta > right) rflag = true;
						rptv = mptv + delta;
						if (rflag == false && m_imgMask[0].getImageIpl()->imageData[rptv] == 0) // not hole
						{
							rflag = true;
							if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[rptv])
							{
								refptv = rptv;
								refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
							}
						}

						delta = v << 1;
						if (delta > right) rrflag = true;
						rrptv = mptv + delta;
						if (rrflag == false && m_imgMask[0].getImageIpl()->imageData[rrptv] == 0) // not hole
						{
							rrflag = true;
							if (refdepth > (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[rrptv])
							{
								refptv = rrptv;
								refdepth = (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[refptv];
							}
						}

						if (llflag == true && lflag == true && lmflag == true && mflag == true && mrflag == true && rflag == true && rrflag == true) break;
					} // v for bottom half

					// inpaint
				//		if(refdepth == 255) continue;	// do nothing
					ref = refptv * 3;
					for (varptv = leftptv + 1; varptv < rightptv; varptv++)
					{
						var = varptv * 3;
						m_imgBlended.getImageIpl()->imageData[var] = m_imgBlended.getImageIpl()->imageData[ref];
						m_imgBlended.getImageIpl()->imageData[var + 1] = m_imgBlended.getImageIpl()->imageData[ref + 1];
						m_imgBlended.getImageIpl()->imageData[var + 2] = m_imgBlended.getImageIpl()->imageData[ref + 2];
					}
				} // else no inpaintflag, do nothing
			} // for w
		} // for h

		// NICT  cvInpaint(m_imgBlended.getImageIpl(), m_imgMask[0].getImageIpl(), m_imgInterpolatedView, 5, CV_INPAINT_NS); // inpaint
		cvErode(m_imgMask[0].getImageIpl(), m_imgMask[0].getImageIpl()); // use pre-inpainted pixels for smoothing
		cvInpaint(m_imgBlended.getImageIpl(), m_imgMask[0].getImageIpl(), m_imgInterpolatedView.getImageIpl(), 3, CV_INPAINT_TELEA); // NICT use small kernel // smooth pre-inpainted area

		//  cvSaveImage("7.bmp", m_imgInterpolatedView.getImageIpl()); // NICT

		cvOr(m_imgMask[1].getImageIpl(), m_imgMask[2].getImageIpl(), m_imgMask[0].getImageIpl()); // filled hole mask

		// NICT Edge filter start
		// Horizontal edge detect
		int varm6, varm5, varm4, varm3, varm2, varm1, var1, var2, var3, var4, var5;

		for (int h = 2; h < cfg.getSourceHeight() - 1; h++)
		{
			holeflag = false;
			filterflag = false;
			hptv = h *  cfg.getSourceWidth();

			for (int w = 2; w < cfg.getSourceWidth() - 1; w++)
			{
				ptv = w + hptv;
				if (holeflag == false && m_imgMask[0].getImageIpl()->imageData[ptv] != 0) // filled hole edge
				{
					holeflag = true;
					if (abs((DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv - 2] - (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv + 1]) < cfg.getDepthBlendDiff())
					{
						filterflag = true;
					}
					else
					{
						filterflag = false;
					}
				}
				else if (holeflag == true && m_imgMask[0].getImageIpl()->imageData[ptv] == 0) // filled hole edge
				{
					holeflag = false;
					// NICT start
					//			if(abs((unsigned char)m_imgBlendedDepth.getImageIpl()->imageData[ptv-2] - (unsigned char)m_imgBlendedDepth.getImageIpl()->imageData[ptv+1]) < cfg.getDepthBlendDiff())
					if (abs((DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv - 2] - (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv + 1]) < cfg.getDepthBlendDiff())
						// NICT end
						filterflag = true;
					else filterflag = false;
				}

				// Horizontal filtering
				if (filterflag == true)
				{
					filterflag = false;
					var = ptv * 3;
					varm6 = var - 6, varm5 = var - 5, varm4 = var - 4, varm3 = var - 3, varm2 = var - 2, varm1 = var - 1, var1 = var + 1, var2 = var + 2, var3 = var + 3, var4 = var + 4, var5 = var + 5;

					m_imgInterpolatedView.getImageIpl()->imageData[var] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm6] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var3]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var1] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm5] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var4]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var2] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm4] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var5]) >> 1;

					m_imgInterpolatedView.getImageIpl()->imageData[varm3] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm6] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[varm2] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm5] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var1]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[varm1] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm4] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var2]) >> 1;

					m_imgInterpolatedView.getImageIpl()->imageData[var3] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var3]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var4] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var1] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var4]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var5] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var2] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var5]) >> 1;
				}
			}
		}

		// Vertical edge detect
		for (int w = 2; w < cfg.getSourceWidth() - 1; w++)
		{
			holeflag = false;
			filterflag = false;
			for (int h = 2; h < cfg.getSourceHeight() - 1; h++)
			{
				ptv = w + h *  cfg.getSourceWidth();
				if (holeflag == false && m_imgMask[0].getImageIpl()->imageData[ptv] != 0) // filled hole edge
				{
					holeflag = true;
					// NICT start
					//			if(abs((unsigned char)m_imgBlendedDepth.getImageIpl()->imageData[ptv-2*cfg.getSourceWidth()] - (unsigned char)m_imgBlendedDepth.getImageIpl()->imageData[ptv+cfg.getSourceWidth()]) < cfg.getDepthBlendDiff())
					if (abs((DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv - 2 * cfg.getSourceWidth()] - (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv + cfg.getSourceWidth()]) < cfg.getDepthBlendDiff())
						filterflag = true;
					else filterflag = false;
				}
				else if (holeflag == true && m_imgMask[0].getImageIpl()->imageData[ptv] == 0) // filled hole edge
				{
					holeflag = false;

					//			if(abs((unsigned char)m_imgBlendedDepth.getImageIpl()->imageData[ptv-2*cfg.getSourceWidth()] - (unsigned char)m_imgBlendedDepth.getImageIpl()->imageData[ptv+cfg.getSourceWidth()]) < cfg.getDepthBlendDiff())
					if (abs((DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv - 2 * cfg.getSourceWidth()] - (DepthType)m_imgBlendedDepth.getImageIpl()->imageData[ptv + cfg.getSourceWidth()]) < cfg.getDepthBlendDiff())
						filterflag = true;
					else filterflag = false;
				}

				// Vertical filtering
				if (filterflag == true)
				{
					filterflag = false;
					var = ptv * 3;
					varm6 = var - 6 * cfg.getSourceWidth(), varm5 = varm6 + 1, varm4 = varm6 + 2, varm3 = var - 3 * cfg.getSourceWidth(), varm2 = varm3 + 1, varm1 = varm3 + 2, var1 = var + 1, var2 = var + 2, var3 = var + 3 * cfg.getSourceWidth(), var4 = var3 + 1, var5 = var3 + 2;

					m_imgInterpolatedView.getImageIpl()->imageData[var] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm6] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var3]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var1] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm5] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var4]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var2] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm4] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var5]) >> 1;

					m_imgInterpolatedView.getImageIpl()->imageData[varm3] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm6] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[varm2] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm5] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var1]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[varm1] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[varm4] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var2]) >> 1;

					m_imgInterpolatedView.getImageIpl()->imageData[var3] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var3]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var4] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var1] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var4]) >> 1;
					m_imgInterpolatedView.getImageIpl()->imageData[var5] = ((unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var2] + (unsigned char)m_imgInterpolatedView.getImageIpl()->imageData[var5]) >> 1;
				}
			}
		}
		// NICT start
	}  // IvsrsInpaint = true
	else
	{
		//cvSaveImage("Mask2.bmp",m_imgMask[0].getImageIpl());
		cvSet(m_imgBlended.getImageIpl(), CV_RGB(0, 128, 128), m_imgMask[0].getImageIpl());
		cvInpaint(m_imgBlended.getImageIpl(), m_imgMask[0].getImageIpl(), m_imgInterpolatedView.getImageIpl(), 5, CV_INPAINT_NS);
		//inpaint(m_imgBlended.getImageIpl(), m_imgMask[0], m_imgInterpolatedView.getImageIpl(), 5, INPAINT_NS);
	}

	if (cfg.getColorSpace()) {
		pSynYuvBuffer->setDataFromImgBGR(m_imgInterpolatedView.getImageIpl());
	}
	else
		pSynYuvBuffer->setDataFromImgYUV(m_imgInterpolatedView.getImageIpl());

	return 0;
}
