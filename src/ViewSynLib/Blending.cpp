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
	Image<DepthType>* synDepthLeft = views[0]->getSynDepth();
	Image<DepthType>* synDepthRight = views[1]->getSynDepth();
	Image<ImageType>* synLeft = views[0]->getSynImage();
	Image<ImageType>* synRight = views[1]->getSynImage();

	double weightLeft = views[0]->getWeight();
	double weightRight = views[1]->getWeight();

	for (int h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (int w = 0; w < cfg.getSourceWidth(); w++)
		{
			int ptv = w + h * cfg.getSourceWidth();

			for (size_t j = 0; j < 3; j++)
			{
				blendedImage->setMatData(ptv * 3 + j, 0);
			}

			if (holesMask->getMatData(ptv) != 0) continue;

			if ((abs(((DepthType*)synDepthLeft->getMat()->imageData[ptv]) - ((DepthType*)synDepthRight->getMat()->imageData[ptv])) < cfg.getDepthBlendDiff())) // left and right are close to each other (NICT)) // left and right are close to each other (NICT)
			{
				((ImageType*)blendedImage->getMat()->imageData)[ptv * 3 + 0] = CLIP3((((ImageType*)synLeft->getMat()->imageData)[ptv * 3 + 0] * weightLeft + ((ImageType*)synRight->getMat()->imageData)[ptv * 3 + 0] * weightRight) / (weightLeft + weightRight), 0, MAX_LUMA - 1);
				((ImageType*)blendedImage->getMat()->imageData)[ptv * 3 + 1] = CLIP3((((ImageType*)synLeft->getMat()->imageData)[ptv * 3 + 1] * weightLeft + ((ImageType*)synRight->getMat()->imageData)[ptv * 3 + 1] * weightRight) / (weightLeft + weightRight), 0, MAX_LUMA - 1);
				((ImageType*)blendedImage->getMat()->imageData)[ptv * 3 + 2] = CLIP3((((ImageType*)synLeft->getMat()->imageData)[ptv * 3 + 2] * weightLeft + ((ImageType*)synRight->getMat()->imageData)[ptv * 3 + 2] * weightRight) / (weightLeft + weightRight), 0, MAX_LUMA - 1);

				if (cfg.getIvsrsInpaint() == 1)
				{
					((DepthType*)blendedDepth->getMat()->imageData)[ptv] = CLIP3((((DepthType*)synDepthLeft->getMat()->imageData)[ptv] * weightLeft + ((DepthType*)synDepthRight->getMat()->imageData)[ptv] * weightRight) / (weightLeft + weightRight), 0, MAX_DEPTH - 1);
				}
			}
			else if ((((DepthType*)synDepthLeft->getMatData(ptv)) > ((DepthType*)synDepthRight->getMatData(ptv)))) //Fix to compare z // left is nearer (NICT)
			{
				blendNearerPixel(blendedImage, blendedDepth, synLeft, synDepthLeft, ptv);
			}
			else /*if((m_imgMask[1]->getMat()->imageData[ptv]!=0))*/ //Fix should be mixed together // Right is closer
			{
				blendNearerPixel(blendedImage, blendedDepth, synRight, synDepthRight, ptv);
			}
		}
	}
}