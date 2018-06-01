#include "BlendingHoles.h"

void BlendingHoles::initHolesMasks(vector<shared_ptr<View>> views, shared_ptr<Image<ImageType>> holesMask)
{
	if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEPTH_BASED)
	{
		cvAnd(views[0]->getSynHolesMat(), views[1]->getSynFillsMat(), views[0]->getFillableHoles()->getMat()); // NICT use
		cvAnd(views[1]->getSynHolesMat(), views[0]->getSynFillsMat(), views[1]->getFillableHoles()->getMat());
	}
	else
	{
		cvAnd(views[0]->getUnstablePixelsMat(), views[1]->getSynFillsMat(), views[0]->getFillableHoles()->getMat()); // Left dilated Mask[0] * Right Success -> Left Mask[3] // dilated hole fillable by Right
		cvAnd(views[1]->getUnstablePixelsMat(), views[0]->getSynFillsMat(), views[1]->getFillableHoles()->getMat()); // Right dilated Mask[0] * Left Success -> Mask[4] // dilated hole fillable by Left
	}

	// pixels which couldn't be synthesized from both left and right -> inpainting
	cvAnd(views[0]->getSynHolesMat(), views[1]->getSynHolesMat(), holesMask->getMat()); // Left Hole * Right Hole -> Mask[2] // common hole,
}