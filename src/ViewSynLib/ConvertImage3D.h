#pragma once

#include "ConvertImage.h"
#include "ViewSynthesis3D.h"

class ConvertImage3D : public ConvertImage
{
public:
	ConvertImage3D(ViewSynthesis3D* viewSynthesis3D, int height, int width);

	~ConvertImage3D()
	{
	}
	
	//! copy the IplImage data to 1D buffers
	void apply(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight, ImageData<HoleType>& pRefHoleLeft, ImageData<HoleType>& pRefHoleRight);

private:
	ViewSynthesis3D* m_viewSynthesis3D;
};
