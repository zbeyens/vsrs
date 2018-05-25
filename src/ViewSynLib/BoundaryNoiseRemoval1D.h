#pragma once

#include "BoundaryNoiseRemoval.h"

class BoundaryNoiseRemoval1D : public BoundaryNoiseRemoval
{
public:
	BoundaryNoiseRemoval1D();
	~BoundaryNoiseRemoval1D();

private:
	void calcWeight();
	void calcDepthThreshold(bool ViewID);
	void Blending(Image<ImageType>* pLeft, Image<ImageType>* pRight, unique_ptr<Image<ImageType>>& outImg);
	void RemainingHoleFilling(Image<ImageType>* pSrc);
	void HoleFillingWithExpandedHole(Image<ImageType>* pSrc, Image<ImageType>* pTar, IplImage* m_imgExpandedHole);
};
