#include "Blending.h"

#ifndef CLIP3
#define CLIP3(x,min,max) ( (x)<(min)?(min):((x)>(max)?(max):(x)) )
#endif

bool Blending::isPixelNearer(DepthType left, DepthType right)
{
	return left > right;
}

void Blending::blendNearerPixel(Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* image, Image<DepthType>* depth, int ptv)
{
	for (size_t j = 0; j < 3; j++)
	{
		blendedImage->setMatData(ptv * 3 + j, image->getMatData(ptv * 3 + j));
	}

	if (cfg.getIvsrsInpaint() == 1)
	{
		blendedDepth->setMatData(ptv, depth->getMatData(ptv));
	}
}

void Blending::apply(vector<View*> views, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask, double totalBaseline)
{

	IplImage *DepthLeft = views[0]->getSynDepth()->getMat();
	IplImage *DepthRight = views[1]->getSynDepth()->getMat();
	IplImage *ImageLeft = views[0]->getSynImage()->getMat();
	IplImage *ImageRight = views[1]->getSynImage()->getMat();

	double weightLeft = views[0]->getWeight();
	double weightRight = views[1]->getWeight();

	for (int h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (int w = 0; w < cfg.getSourceWidth(); w++)
		{
			int ptv = w + h * cfg.getSourceWidth();
			blendedImage->getMat()->imageData[ptv * 3 + 0] = 0;
			blendedImage->getMat()->imageData[ptv * 3 + 1] = 0;
			blendedImage->getMat()->imageData[ptv * 3 + 2] = 0;
			if (holesMask->getMat()->imageData[ptv] != 0) continue;

			if ((abs(((DepthType*)DepthLeft->imageData[ptv]) - ((DepthType*)DepthRight->imageData[ptv])) < cfg.getDepthBlendDiff())) // left and right are close to each other (NICT)
			{
				((ImageType*)blendedImage->getMat()->imageData)[ptv * 3 + 0] = CLIP3((((ImageType*)ImageLeft->imageData)[ptv * 3 + 0] * weightLeft + ((ImageType*)ImageRight->imageData)[ptv * 3 + 0] * weightRight) / (weightLeft + weightRight), 0, MAX_LUMA - 1);
				((ImageType*)blendedImage->getMat()->imageData)[ptv * 3 + 1] = CLIP3((((ImageType*)ImageLeft->imageData)[ptv * 3 + 1] * weightLeft + ((ImageType*)ImageRight->imageData)[ptv * 3 + 1] * weightRight) / (weightLeft + weightRight), 0, MAX_LUMA - 1);
				((ImageType*)blendedImage->getMat()->imageData)[ptv * 3 + 2] = CLIP3((((ImageType*)ImageLeft->imageData)[ptv * 3 + 2] * weightLeft + ((ImageType*)ImageRight->imageData)[ptv * 3 + 2] * weightRight) / (weightLeft + weightRight), 0, MAX_LUMA - 1);

				if (cfg.getIvsrsInpaint() == 1)
				{
					((DepthType*)blendedDepth->getMat()->imageData)[ptv] = CLIP3((((DepthType*)DepthLeft->imageData)[ptv] * weightLeft + ((DepthType*)DepthRight->imageData)[ptv] * weightRight) / (weightLeft + weightRight), 0, MAX_DEPTH - 1);
				}
			}
			else if ((((DepthType*)DepthLeft->imageData[ptv]) > ((DepthType*)DepthRight->imageData[ptv]))) //Fix to compare z // left is nearer (NICT)
			{
				blendedImage->getMat()->imageData[ptv * 3 + 0] = ImageLeft->imageData[ptv * 3 + 0];
				blendedImage->getMat()->imageData[ptv * 3 + 1] = ImageLeft->imageData[ptv * 3 + 1];
				blendedImage->getMat()->imageData[ptv * 3 + 2] = ImageLeft->imageData[ptv * 3 + 2];

				if (cfg.getIvsrsInpaint() == 1)
				{
					blendedDepth->getMat()->imageData[ptv] = DepthLeft->imageData[ptv];
				}
			}
			else /*if((m_imgMask[1]->getMat()->imageData[ptv]!=0))*/ //Fix should be mixed together // Right is closer
			{
				blendedImage->getMat()->imageData[ptv * 3 + 0] = ImageRight->imageData[ptv * 3 + 0];
				blendedImage->getMat()->imageData[ptv * 3 + 1] = ImageRight->imageData[ptv * 3 + 1];
				blendedImage->getMat()->imageData[ptv * 3 + 2] = ImageRight->imageData[ptv * 3 + 2];

				if (cfg.getIvsrsInpaint() == 1)
				{
					blendedDepth->getMat()->imageData[ptv] = DepthRight->imageData[ptv];
				}
			}
		}
	}


	//for (int h = 0; h < cfg.getSourceHeight(); h++)
	//{
	//	for (int w = 0; w < cfg.getSourceWidth(); w++)
	//	{
	//		int ptv = w + h * cfg.getSourceWidth();

	//		for (size_t j = 0; j < 3; j++)
	//		{
	//			blendedImage->setMatData(ptv * 3 + j, 0);
	//		}

	//		if (holesMask->getMatData(ptv) != 0) continue;

	//		if ((abs((synDepthLeft->getMatData(ptv)) - (synDepthRight->getMatData(ptv))) < cfg.getDepthBlendDiff())) // left and right are close to each other (NICT)
	//		{
	//			for (size_t j = 0; j < 3; j++)
	//			{
	//				blendedImage->setMatData(ptv * 3 + j, ImageTools::CLIP3((synLeft->getMatData(ptv * 3 + j) * weightLeft + synRight->getMatData(ptv * 3 + j) * weightRight) / totalBaseline, 0, MAX_LUMA - 1));
	//			}

	//			if (cfg.getIvsrsInpaint() == 1)
	//			{
	//				blendedDepth->setMatData(ptv, ImageTools::CLIP3((synDepthLeft->getMatData(ptv) * weightLeft + synDepthRight->getMatData(ptv) * weightRight) / totalBaseline, 0, MAX_DEPTH - 1));
	//			}
	//		}
	//		else if (isPixelNearer(synDepthLeft->getMatData(ptv), synDepthRight->getMatData(ptv)))
	//		{
	//			blendNearerPixel(blendedImage, blendedDepth, synLeft, synDepthLeft, ptv);
	//		}
	//		else
	//		{
	//			blendNearerPixel(blendedImage, blendedDepth, synRight, synDepthRight, ptv);
	//		}
	//	}
	//}
}
