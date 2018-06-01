#include "Blending2V.h"

void Blending2V::blendPixel(shared_ptr<Image<ImageType>> outImage, shared_ptr<Image<DepthType>> outDepth, shared_ptr<Image<ImageType>> inImage, shared_ptr<Image<DepthType>> inDepth, int ptv)
{
	for (size_t j = 0; j < 3; j++)
	{
		outImage->setMatData(ptv * 3 + j, inImage->getMatData(ptv * 3 + j));
	}

	if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEPTH_BASED)
	{
		outDepth->setMatData(ptv, inDepth->getMatData(ptv));
	}
}

void Blending2V::apply(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> outImage, shared_ptr<Image<DepthType>> outDepth, shared_ptr<Image<ImageType>> holesMask)
{
	shared_ptr<Image<DepthType>> synDepthLeft = views[0]->getSynDepth();
	shared_ptr<Image<DepthType>> synDepthRight = views[1]->getSynDepth();
	shared_ptr<Image<ImageType>> synLeft = views[0]->getSynImage();
	shared_ptr<Image<ImageType>> synRight = views[1]->getSynImage();

	double weightLeft = views[0]->getWeight();
	double weightRight = views[1]->getWeight();

	for (int h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (int w = 0; w < cfg.getSourceWidth(); w++)
		{
			int ptv = w + h * cfg.getSourceWidth();

			for (size_t j = 0; j < 3; j++)
			{
				outImage->setMatData(ptv * 3 + j, 0);
			}

			if (holesMask->getMatData(ptv) != 0) continue;

			if ((abs(((DepthType*)synDepthLeft->getMat()->imageData[ptv]) - ((DepthType*)synDepthRight->getMat()->imageData[ptv])) < cfg.getDepthBlendDiff())) // left and right are close to each other (NICT)) // left and right are close to each other (NICT)
			{
				for (size_t i = 0; i < 3; i++)
				{
					((ImageType*)outImage->getMat()->imageData)[ptv * 3 + i] = ImageTools::CLIP3((((ImageType*)synLeft->getMat()->imageData)[ptv * 3 + i] * weightLeft + ((ImageType*)synRight->getMat()->imageData)[ptv * 3 + i] * weightRight) / (weightLeft + weightRight), 0, MAX_LUMA - 1);
				}

				//if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEPTH_BASED)
				((DepthType*)outDepth->getMat()->imageData)[ptv] = ImageTools::CLIP3((((DepthType*)synDepthLeft->getMat()->imageData)[ptv] * weightLeft + ((DepthType*)synDepthRight->getMat()->imageData)[ptv] * weightRight) / (weightLeft + weightRight), 0, MAX_DEPTH - 1);
			}
			else if ((((DepthType*)synDepthLeft->getMatData(ptv)) > ((DepthType*)synDepthRight->getMatData(ptv)))) // Left is closer
			{
				blendPixel(outImage, outDepth, synLeft, synDepthLeft, ptv);
			}
			else // Right is closer
			{
				blendPixel(outImage, outDepth, synRight, synDepthRight, ptv);
			}
		}
	}
}