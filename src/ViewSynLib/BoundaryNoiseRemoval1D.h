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
	void Blending(shared_ptr<Image<ImageType>> pLeft, shared_ptr<Image<ImageType>> pRight, shared_ptr<Image<ImageType>> outImg);
	void RemainingHoleFilling(shared_ptr<Image<ImageType>> pSrc);
	void HoleFillingWithExpandedHole(shared_ptr<Image<ImageType>> pSrc, shared_ptr<Image<ImageType>> pTar, IplImage* m_imgExpandedHole);
};
