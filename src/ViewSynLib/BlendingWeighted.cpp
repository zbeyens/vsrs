#include "BlendingWeighted.h"

void BlendingWeighted::apply(vector<View*> views, Image<ImageType>* holesMask)
{
	initHolesMasks(views, holesMask);

	Image<ImageType>* pRefLeft = views[0]->getSynImage();
	Image<ImageType>* pRefRight = views[1]->getSynImage();
	Image<DepthType>* pRefDepthLeft = views[0]->getSynDepth();
	Image<DepthType>* pRefDepthRight = views[1]->getSynDepth();
	Image<HoleType>* pRefFillsLeft = views[0]->getSynFills();
	Image<HoleType>* pRefFillsRight = views[1]->getSynFills();

	cvCopy(pRefRight->getMat(), pRefLeft->getMat(), views[0]->getFillableHoles()->getMat()); // Right VirtualImage * hole fillable by Right -> Left VirtualImage // Left hole is filled by Right
	cvCopy(pRefLeft->getMat(), pRefRight->getMat(), views[1]->getFillableHoles()->getMat()); // Left VirtualImage * hole fillable by Left ->Right VirtualImage
	if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEPTH_BASED)
	{
		cvCopy(pRefDepthRight->getMat(), pRefDepthLeft->getMat(), views[0]->getFillableHoles()->getMat()); // Right VirtualDepth * hole fillabl by Right -> Left VirtualDepth //NICT
		cvCopy(pRefDepthLeft->getMat(), pRefDepthRight->getMat(), views[1]->getFillableHoles()->getMat()); // Left VirtualDedpth * hole fillable by Left ->Right VirtualDepth // NICT
	}
}