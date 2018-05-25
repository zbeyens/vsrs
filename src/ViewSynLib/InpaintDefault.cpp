#include "InpaintDefault.h"


void InpaintDefault::apply(Image<ImageType>* outputImage, Image<ImageType>* inputImage, Image<DepthType>* inputDepth, Image<ImageType>* holesMask, vector<View*> views)
{
	cvSet(inputImage->getMat(), CV_RGB(0, 128, 128), holesMask->getMat());
	cvInpaint(inputImage->getMat(), holesMask->getMat(), outputImage->getMat(), 5, CV_INPAINT_NS);
}
