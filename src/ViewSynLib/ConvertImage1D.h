#pragma once

#include "ConvertImage.h"
#include "ViewSynthesis1D.h"

class ConvertImage1D : public ConvertImage
{
public:
	ConvertImage1D(ViewSynthesis1D* viewSynthesis1D, int height, int width);

	~ConvertImage1D()
	{
	}

	//!	copy the synthesized YUV data to ImageData (buffer1D)
	void apply(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight);

private:
	ViewSynthesis1D* m_viewSynthesis1D;
};
