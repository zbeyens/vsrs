#include "InpaintDefault.h"


void InpaintDefault::apply(Image<ImageType>* synImage, Image<ImageType>* blendedImage, Image<DepthType>* blendedDepth, Image<ImageType>* holesMask, vector<View*> views)
{
	cvSet(blendedImage->getMat(), CV_RGB(0, 128, 128), holesMask->getMat());
	cvInpaint(blendedImage->getMat(), holesMask->getMat(), synImage->getMat(), 5, CV_INPAINT_NS);
}
