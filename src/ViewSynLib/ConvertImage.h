#pragma once

#include "ImageData.h"
#include "ConfigSyn.h"

class ConvertImage
{
public:
	ConvertImage()
	{
	}

	ConvertImage(int height, int width)
		: m_height(height)
		, m_width(width)
	{
	}

	~ConvertImage()
	{
	}

	virtual void apply(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight, ImageData<HoleType>& pRefHoleLeft, ImageData<HoleType>& pRefHoleRight) {}
	virtual void apply(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight) {}

protected:
	int m_height, m_width;

};
