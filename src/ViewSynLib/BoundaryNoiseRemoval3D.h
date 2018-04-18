#pragma once

#include "BoundaryNoiseRemoval.h"

class BoundaryNoiseRemoval3D : public BoundaryNoiseRemoval
{
public:
	BoundaryNoiseRemoval3D();
	~BoundaryNoiseRemoval3D();

private:
	void calcWeight();
	void Blending(Image<ImageType>* pLeft, Image<ImageType>* pRight, unique_ptr<Image<ImageType>>& pSyn, bool SynthesisMode);
	void RemainingHoleFilling(Image<ImageType>* pSrc);
	void HoleFillingWithExpandedHole(Image<ImageType>* pSrc, Image<ImageType>* pTar, IplImage* m_imgExpandedHole, bool SynthesisMode);
};
