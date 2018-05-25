#include "BlendingCloser.h"


void BlendingCloser::apply(vector<View*> views, Image<ImageType>* holesMask)
{
	initHolesMasks(views, holesMask);

	Image<ImageType>* pRefLeft = views[0]->getSynImage();
	Image<ImageType>* pRefRight = views[1]->getSynImage();
	Image<DepthType>* pRefDepthLeft = views[0]->getSynDepth();
	Image<DepthType>* pRefDepthRight = views[1]->getSynDepth();
	Image<HoleType>* pRefFillsLeft = views[0]->getSynFills();
	Image<HoleType>* pRefFillsRight = views[1]->getSynFills();

	if (views[0]->getWeight() >= views[1]->getWeight())  // if closer to Left, fill the left holes and put the left fills on the right
	{
		cvCopy(pRefRight->getMat(), pRefLeft->getMat(), views[0]->getFillableHoles()->getMat());  // Right * Mask[0] -> Left // dilated hole may left
		cvCopy(pRefLeft->getMat(), pRefRight->getMat(), pRefFillsLeft->getMat()); // Left VirtualImage * success -> Right VirtualImage
	}
	else                               // if closer to Right
	{
		cvCopy(pRefRight->getMat(), pRefLeft->getMat(), pRefFillsRight->getMat()); // Right VirtualImage * success -> Left VirtualImage
		cvCopy(pRefLeft->getMat(), pRefRight->getMat(), views[1]->getFillableHoles()->getMat()); // Left VirtualImage * Mask[1] -> Right VirtualImage
	}
}
