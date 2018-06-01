#include "BlendingCloser.h"


void BlendingCloser::apply(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> holesMask)
{
	initHolesMasks(views, holesMask);

	shared_ptr<Image<ImageType>> pRefLeft = views[0]->getSynImage();
	shared_ptr<Image<ImageType>> pRefRight = views[1]->getSynImage();
	shared_ptr<Image<DepthType>> pRefDepthLeft = views[0]->getSynDepth();
	shared_ptr<Image<DepthType>> pRefDepthRight = views[1]->getSynDepth();
	shared_ptr<Image<HoleType>> pRefFillsLeft = views[0]->getSynFills();
	shared_ptr<Image<HoleType>> pRefFillsRight = views[1]->getSynFills();

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
