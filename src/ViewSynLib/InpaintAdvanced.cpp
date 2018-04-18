#include "InpaintAdvanced.h"

void InpaintAdvanced::apply(Image<ImageType>* synImage, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask, vector<View*> views)
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
			if (holesMask->getMatData(ptv) != 0) // hole
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
						refdepth = (DepthType)blendedDepth->getMatData(refptv); // set left depth to ref depth
					}// else, left was set before
				}
				else if (holeflag == false) // hole start at middle
				{
					holeflag = true;
					leftw = w - 1;
					leftptv = ptv - 1;
					refptv = leftptv;
					refdepth = (DepthType)blendedDepth->getMatData(refptv); // set left depth to ref depth
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
					if (refdepth > (DepthType)blendedDepth->getMatData(rightptv)) // set right depth to ref depth
					{
						refptv = rightptv;
						refdepth = (DepthType)blendedDepth->getMatData(refptv);
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
						if (mflag == false && holesMask->getMatData(mptv) == 0) // not hole
						{
							mflag = true;
							if (refdepth > (DepthType) blendedDepth->getMatData(mptv))
							{
								refptv = mptv;
								refdepth = (DepthType)blendedDepth->getMatData(refptv);
							}
						} // else hole, do nothing

						delta = v << 1;
						if (delta > left) llflag = true;
						llptv = mptv - delta;
						if (llflag == false && holesMask->getMatData(llptv) == 0) // not hole
						{
							llflag = true;
							if (refdepth > (DepthType)blendedDepth->getMatData(llptv))
							{
								refptv = llptv;
								refdepth = (DepthType)blendedDepth->getMatData(refptv);
							}
						} // else hole, do nopthing

						delta = v;
						if (delta > left) lflag = true;
						lptv = mptv - delta;
						if (lflag == false && holesMask->getMatData(lptv) == 0) // not hole
						{
							lflag = true;
							if (refdepth > (DepthType)blendedDepth->getMatData(lptv))
							{
								refptv = lptv;
								refdepth = (DepthType)blendedDepth->getMatData(refptv);
							}
						} // else hole, do nopthing

						delta = v >> 1;
						if (delta > left) lmflag = true;
						lmptv = mptv - delta;
						if (lmflag == false && holesMask->getMatData(lmptv) == 0) // not hole
						{
							lmflag = true;
							if (refdepth > (DepthType)blendedDepth->getMatData(lmptv))
							{
								refptv = lmptv;
								refdepth = (DepthType)blendedDepth->getMatData(refptv);
							}
						} // else hole, do nopthing

						delta = v >> 1;
						if (delta > right) mrflag = true;
						mrptv = mptv + delta;
						if (mrflag == false && holesMask->getMatData(mrptv) == 0) // not hole
						{
							mrflag = true;
							if (refdepth > (DepthType)blendedDepth->getMatData(mrptv))
							{
								refptv = mrptv;
								refdepth = (DepthType)blendedDepth->getMatData(refptv);
							}
						} // else hole, do nothing

						delta = v;

						if (delta > right) rflag = true;
						rptv = mptv + delta;
						if (rflag == false && holesMask->getMatData(rptv) == 0) // not hole
						{
							rflag = true;
							if (refdepth > (DepthType)blendedDepth->getMatData(rptv))
							{
								refptv = rptv;
								refdepth = (DepthType)blendedDepth->getMatData(refptv);
							}
						} // else hole, do nothing

						delta = v << 1;
						if (delta > right) rrflag = true;
						rrptv = mptv + delta;
						if (rrflag == false && holesMask->getMatData(rrptv) == 0) // not hole
						{
							rrflag = true;
							if (refdepth > (DepthType)blendedDepth->getMatData(rrptv))
							{
								refptv = rrptv;
								refdepth = (DepthType)blendedDepth->getMatData(refptv);
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
					if (mflag == false && holesMask->getMatData(mptv) == 0) // not hole
					{
						mflag = true;
						if (refdepth > (DepthType) blendedDepth->getMatData(mptv))
						{
							refptv = mptv;
							refdepth = (DepthType)blendedDepth->getMatData(refptv);
						}
					}

					delta = v << 1;
					if (delta > left) llflag = true;
					llptv = mptv - delta;

					if (llflag == false && holesMask->getMatData(llptv) == 0) // not hole
					{
						llflag = true;
						if (refdepth > (DepthType)blendedDepth->getMatData(llptv))
						{
							refptv = llptv;
							refdepth = (DepthType)blendedDepth->getMatData(refptv);
						}
					}

					delta = v;
					if (delta > left) lflag = true;
					lptv = mptv - delta;
					if (lflag == false && holesMask->getMatData(lptv) == 0) // not hole
					{
						lflag = true;
						if (refdepth > (DepthType)blendedDepth->getMatData(lptv))
						{
							refptv = lptv;
							refdepth = (DepthType)blendedDepth->getMatData(refptv);
						}
					}

					delta = v >> 1;
					if (delta > left) lmflag = true;
					lmptv = mptv - delta;
					if (lmflag == false && holesMask->getMatData(lmptv) == 0) // not hole
					{
						lmflag = true;
						if (refdepth > (DepthType)blendedDepth->getMatData(lmptv))
						{
							refptv = lmptv;
							refdepth = (DepthType)blendedDepth->getMatData(refptv);
						}
					}

					delta = v >> 1;
					if (delta > right) mrflag = true;
					mrptv = mptv + delta;
					if (mrflag == false && holesMask->getMatData(mrptv) == 0) // not hole
					{
						mrflag = true;
						if (refdepth > (DepthType)blendedDepth->getMatData(mrptv))
						{
							refptv = mrptv;
							refdepth = (DepthType)blendedDepth->getMatData(refptv);
						}
					}

					delta = v;
					if (delta > right) rflag = true;
					rptv = mptv + delta;
					if (rflag == false && holesMask->getMatData(rptv) == 0) // not hole
					{
						rflag = true;
						if (refdepth > (DepthType)blendedDepth->getMatData(rptv))
						{
							refptv = rptv;
							refdepth = (DepthType)blendedDepth->getMatData(refptv);
						}
					}

					delta = v << 1;
					if (delta > right) rrflag = true;
					rrptv = mptv + delta;
					if (rrflag == false && holesMask->getMatData(rrptv) == 0) // not hole
					{
						rrflag = true;
						if (refdepth > (DepthType)blendedDepth->getMatData(rrptv))
						{
							refptv = rrptv;
							refdepth = (DepthType)blendedDepth->getMatData(refptv);
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
					blendedImage->setMatData(var, blendedImage->getMatData(ref));
					blendedImage->setMatData(var + 1, blendedImage->getMatData(ref + 1));
					blendedImage->setMatData(var + 2, blendedImage->getMatData(ref + 2));
				}
			} // else no inpaintflag, do nothing
		} // for w
	} // for h

	  // NICT  cvInpaint(imgBlended.getImageIpl(), imgMask[0].getImageIpl(), imgInterpolatedView, 5, CV_INPAINT_NS); // inpaint
	cvErode(holesMask->getMat(), holesMask->getMat()); // use pre-inpainted pixels for smoothing
	cvInpaint(blendedImage->getMat(), holesMask->getMat(), synImage->getMat(), 3, CV_INPAINT_TELEA); // NICT use small kernel // smooth pre-inpainted area

	//cvZero(holesMask->getMat());

		cvOr(views[0]->getFillableHoles()->getMat(), views[1]->getFillableHoles()->getMat(), holesMask->getMat()); // filled hole mask
	//for (size_t i = 0; i < cfg.getNView(); i++)
	//{
	//	cvOr(views[i]->getFillableHoles()->getMat(), holesMask->getMat(), holesMask->getMat()); // filled hole mask
	//}
	cout << "coucou";

	// NICT Edge filter start
	// Horizontal edge detect
	int varm6, varm5, varm4, varm3, varm2, varm1, var1, var2, var3, var4, var5;

	for (int h = 2; h < cfg.getSourceHeight() - 1; h++)
	{
		holeflag = false;
		filterflag = false;
		hptv = h * cfg.getSourceWidth();

		for (int w = 2; w < cfg.getSourceWidth() - 1; w++)
		{
			ptv = w + hptv;
			if (holeflag == false && holesMask->getMatData(ptv) != 0) // filled hole edge
			{
				holeflag = true;
				if (abs((DepthType)blendedDepth->getMatData(ptv - 2) - (DepthType)blendedDepth->getMatData(ptv + 1)) < cfg.getDepthBlendDiff())
				{
					filterflag = true;
				}
				else
				{
					filterflag = false;
				}
			}
			else if (holeflag == true && holesMask->getMatData(ptv) == 0) // filled hole edge
			{
				holeflag = false;
				// NICT start
				//			if(abs((unsigned char)imgBlendedDepth->getImageIpl()->imageData[ptv-2] - (unsigned char)imgBlendedDepth->getImageIpl()->imageData[ptv+1]) < cfg.getDepthBlendDiff())
				if (abs((DepthType)blendedDepth->getMatData(ptv - 2) - (DepthType)blendedDepth->getMatData(ptv + 1)) < cfg.getDepthBlendDiff())
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

				synImage->setMatData(var, ((unsigned char)synImage->getMatData(varm6) + (unsigned char)synImage->getMatData(var3)) >> 1);
				synImage->setMatData(var1, ((unsigned char)synImage->getMatData(varm5) + (unsigned char)synImage->getMatData(var4)) >> 1);
				synImage->setMatData(var2, ((unsigned char)synImage->getMatData(varm4) + (unsigned char)synImage->getMatData(var5)) >> 1);

				synImage->setMatData(varm3, ((unsigned char)synImage->getMatData(varm6) + (unsigned char)synImage->getMatData(var)) >> 1);
				synImage->setMatData(varm2, ((unsigned char)synImage->getMatData(varm5) + (unsigned char)synImage->getMatData(var1)) >> 1);
				synImage->setMatData(varm1, ((unsigned char)synImage->getMatData(varm4) + (unsigned char)synImage->getMatData(var2)) >> 1);

				synImage->setMatData(var3, ((unsigned char)synImage->getMatData(var) + (unsigned char)synImage->getMatData(var3)) >> 1);
				synImage->setMatData(var4, ((unsigned char)synImage->getMatData(var1) + (unsigned char)synImage->getMatData(var4)) >> 1);
				synImage->setMatData(var5, ((unsigned char)synImage->getMatData(var2) + (unsigned char)synImage->getMatData(var5)) >> 1);
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
			ptv = w + h * cfg.getSourceWidth();
			if (holeflag == false && holesMask->getMatData(ptv) != 0) // filled hole edge
			{
				holeflag = true;

				if (abs((DepthType)blendedDepth->getMatData(ptv - 2 * cfg.getSourceWidth()) - (DepthType)blendedDepth->getMatData(ptv + cfg.getSourceWidth())) < cfg.getDepthBlendDiff())
					filterflag = true;
				else filterflag = false;
			}
			else if (holeflag == true && holesMask->getMatData(ptv) == 0) // filled hole edge
			{
				holeflag = false;

				if (abs((DepthType)blendedDepth->getMatData(ptv - 2 * cfg.getSourceWidth()) - (DepthType)blendedDepth->getMatData(ptv + cfg.getSourceWidth())) < cfg.getDepthBlendDiff())
					filterflag = true;
				else filterflag = false;
			}

			// Vertical filtering
			if (filterflag == true)
			{
				filterflag = false;
				var = ptv * 3;
				varm6 = var - 6 * cfg.getSourceWidth(), varm5 = varm6 + 1, varm4 = varm6 + 2, varm3 = var - 3 * cfg.getSourceWidth(), varm2 = varm3 + 1, varm1 = varm3 + 2, var1 = var + 1, var2 = var + 2, var3 = var + 3 * cfg.getSourceWidth(), var4 = var3 + 1, var5 = var3 + 2;

				synImage->setMatData(var, ((unsigned char)synImage->getMatData(varm6) + (unsigned char)synImage->getMatData(var3)) >> 1);
				synImage->setMatData(var1, ((unsigned char)synImage->getMatData(varm5) + (unsigned char)synImage->getMatData(var4)) >> 1);
				synImage->setMatData(var2, ((unsigned char)synImage->getMatData(varm4) + (unsigned char)synImage->getMatData(var5)) >> 1);

				synImage->setMatData(varm3, ((unsigned char)synImage->getMatData(varm6) + (unsigned char)synImage->getMatData(var)) >> 1);
				synImage->setMatData(varm2, ((unsigned char)synImage->getMatData(varm5) + (unsigned char)synImage->getMatData(var1)) >> 1);
				synImage->setMatData(varm1, ((unsigned char)synImage->getMatData(varm4) + (unsigned char)synImage->getMatData(var2)) >> 1);

				synImage->setMatData(var3, ((unsigned char)synImage->getMatData(var) + (unsigned char)synImage->getMatData(var3)) >> 1);
				synImage->setMatData(var4, ((unsigned char)synImage->getMatData(var1) + (unsigned char)synImage->getMatData(var4)) >> 1);
				synImage->setMatData(var5, ((unsigned char)synImage->getMatData(var2) + (unsigned char)synImage->getMatData(var5)) >> 1);
			}
		}
	}
}