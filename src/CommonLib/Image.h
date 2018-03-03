#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "Tools.h"

template <typename PixelType>
class Image
{
public:
	Image()
	{
		imageIpl = NULL;
		imageY = imageU = imageV = NULL;
	}
	~Image()
	{
		Tools::safeReleaseImage(imageIpl);

		Tools::safeFree(imageY);
		Tools::safeFree(imageU);
		Tools::safeFree(imageV);
	}

	/** Creates an image header and allocates the image data.
		3th param: depth Bit depth of image elements = 8 * size of each pixel
		@param w Image width
		@param h Image height
		@param channels Number of channels per pixel. Creates images with interleaved channels.
	*/
	void create(int w, int h, int channels) {
		imageIpl = cvCreateImage(cvSize(w, h), IMAGE_DEPTH * sizeof(PixelType), channels);
	}
	void setZero() {
		cvZero(imageIpl);
	}

	bool initYUV(int widthStep)
	{
		int sw = cfg.getSourceWidth();
		int sh = cfg.getSourceHeight();

		// Nagoya start
		if ((imageY = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL
			|| (imageU = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL
			|| (imageV = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL) return false;

		//alloc imageY, U, V
		for (int h = 0; h < sh; h++) 
		{
			if ((imageY[h] = (PixelType *)malloc(widthStep * sizeof(PixelType))) == NULL
				|| (imageU[h] = (PixelType *)malloc(widthStep * sizeof(PixelType))) == NULL
				|| (imageV[h] = (PixelType *)malloc(widthStep * sizeof(PixelType))) == NULL) return false;
		}

		//store imageY, U, V - format image->imageData: [YUVYUVYUV...]
		for (int h = 0; h < sh; h++)
		{
			for (uint w = 0; w < sw; w++)
			{
				imageY[h][w] = (PixelType)(imageIpl->imageData[(h*sw + w) * 3]);
				imageU[h][w] = (PixelType)(imageIpl->imageData[(h*sw + w) * 3 + 1]);
				imageV[h][w] = (PixelType)(imageIpl->imageData[(h*sw + w) * 3 + 2]);
			}
		}
		// Nagoya end

		return true;
	}

	/**
		used for 1 channel
	*/
	bool initY()
	{
		int sh = cfg.getSourceHeight();

		if ((imageY = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL) return false;

		//h	= position in height
		//pos = position in pixel, increment line width
		uint h, pos;
		for (h = pos = 0; h < sh; h++, pos += imageIpl->widthStep)
		{
			imageY[h] = (PixelType *) &(imageIpl->imageData[pos]);
		}

		return true;
	}

	IplImage* getImageIpl() { return imageIpl; }
	PixelType** getImageY() { return imageY; }
	PixelType** getImageU() { return imageU; }
	PixelType** getImageV() { return imageV; }

private:
	ConfigSyn & cfg = ConfigSyn::getInstance();

	IplImage * imageIpl;

	//Nagoya start
	PixelType** imageY;
	PixelType** imageU;
	PixelType** imageV;
	//Nagoya end
};