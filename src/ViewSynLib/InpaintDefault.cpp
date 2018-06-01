#include "InpaintDefault.h"


void InpaintDefault::apply(shared_ptr<Image<ImageType>> outImage, shared_ptr<Image<ImageType>> inImage, shared_ptr<Image<DepthType>> inDepth, shared_ptr<Image<ImageType>> holesMask, vector<shared_ptr<View>> views)
{
	cvSet(inImage->getMat(), CV_RGB(0, 128, 128), holesMask->getMat());
	cvInpaint(inImage->getMat(), holesMask->getMat(), outImage->getMat(), 5, CV_INPAINT_NS);
}
