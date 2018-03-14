/***********************************************************

			 YUV image input/output functions

 ***********************************************************/
#ifndef _INCLUDE_YUV_H_
#define _INCLUDE_YUV_H_

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "ImageResample.h"
#include "Image.h"
#include "ImageTools.h"

#include "FilterDummy2D.h"
#include "FilterHorizontalAVCHalf2D.h"
#include "FilterHorizontalAVCQuarter2D.h"
#include "FilterHorizontalCubicHalf2D.h"
#include "FilterHorizontalCubicQuarter2D.h"
#include "FilterHorizontalLinearHalf2D.h"
#include "FilterHorizontalLinearQuarter2D.h"

/**
	Class containing the pixel values in a YUV or BGR format
	With an eventual upsampling
*/
template <class PixelType>
class ImageData
{
public:
	ImageData();
	ImageData(int h, int w, int chroma_format);
	~ImageData();

	void init(int h, int w, int chroma_format);

	bool      resize(int h, int w, int chroma_format);
	PixelType* getFrame();	//!< get frame Y[0]

	void      setDataFromImgIplBGR(IplImage *imgBGR);
	void      setDataFromImgIplYUV(IplImage *imgYUV);

	bool      setImage444DataToBGR(ImageData<PixelType> *yuvSrc);	//!< set input image in BGR
	bool      setImage444DataToYUV(ImageData<PixelType> *yuvSrc);	//!< set input image in YUV

	bool      setUpsampleFilter(unsigned int filterConfig, unsigned int precision);	//!< select the upsample filter
	bool      upsampling(ImageData<PixelType> src, int padding_size = 0);	//!< apply upsample filter on Y, U, V

	int getSampling() { return sampling; }
	int getHeight() { return height; }
	int getWidth() { return width; }
	int getHeightUV() { return heightUV; }
	int getWidthUV() { return widthUV; }
	int	getSize() { return m_sizeByte; }
	PixelType***    getData() { return m_comp; }
	PixelType*      getBuffer1D() { return m_buffer1D; }

	PixelType **Y;
	PixelType **U;
	PixelType **V;

private:
	void prealloc();
	/**
		malloc comp (Y, U, V) and the 1D buffer
	*/
	bool      allocate_mem();
	void      release_mem();

	ConfigSyn& cfg = ConfigSyn::getInstance();
	int height;
	int width;
	int heightUV;		//!< height of U and V depending on chroma format
	int widthUV;		//!< width of U and V depending on chroma format
	int picsizeY;		//!< pic size for Y: height*width
	int picsizeUV;		//!< pic size for each U and V: heightUV*widthUV
	int m_sizeByte;		//!< YUV size in bytes: picsizeY + picsizeUV * 2
	int sampling;		//!< chroma format: 444, 422, 420 or 400

	//upsampling filter function depending on the cfg.filter and cfg.precision
	Filter<PixelType>* filter;

	//= Y**U**V** , put YUV or BGR data depending on ColorSpace
	PixelType      **m_comp[3];

	PixelType      *m_buffer1D;  //!> 1D array for the Y, U and V component.
};

template <class PixelType>
ImageData<PixelType>::ImageData()
{
	prealloc();
}

template <class PixelType>
ImageData<PixelType>::ImageData(int h, int w, int chroma_format)
{
	init(h, w, chroma_format);
}


template <class PixelType>
void ImageData<PixelType>::init(int h, int w, int chroma_format)
{
	prealloc();

	resize(h, w, chroma_format);

	int precision;
	if (cfg.getSynthesisMode() == 0) // General mode
	{
		precision = cfg.getPrecision();
	}
	else // 1D mode: No upsampling is done before going into Visbd
	{
		precision = 1;
	}
	setUpsampleFilter(cfg.getFilter(), precision);
}

template <class PixelType>
ImageData<PixelType>::~ImageData()
{
	release_mem();
}

template <class PixelType>
void ImageData<PixelType>::prealloc()
{
	Y = U = V = NULL;
	m_comp[0] = m_comp[1] = m_comp[2] = NULL;
	m_buffer1D = NULL;
	filter = NULL;
}

template <class PixelType>
bool ImageData<PixelType>::resize(int h, int w, int chroma_format)
{
	height = h;
	width = w;
	sampling = chroma_format;

	switch (sampling)
	{
	case 400: // padding chroma components
		heightUV = 0;
		widthUV = 0;
		break;
	case 420:
		heightUV = height / 2;
		widthUV = width / 2;
		break;
	case 422:
		heightUV = height;
		widthUV = width / 2;
		break;
	case 444:
		heightUV = height;
		widthUV = width;
		break;
	default:
		fprintf(stderr, "Unknown chroma sampling\n");
		Y = U = V = NULL;
		return false;
	}

	picsizeY = height * width;
	picsizeUV = heightUV * widthUV;
	m_sizeByte = (picsizeY + picsizeUV * 2) * sizeof(PixelType);

	return allocate_mem();
}

template <class PixelType>
bool ImageData<PixelType>::allocate_mem()
{
	int h, pos;
	// size_in_byte elem
	PixelType *buf1D;

	// (height + heightUV * 2) rows, Y (0) - U (height) - V (height + heightUV)
	PixelType **buf2D;

	release_mem();

	if ((buf1D = (PixelType *)malloc(m_sizeByte)) == NULL) return false;

	if ((buf2D = (PixelType **)malloc((height + heightUV * 2) * sizeof(PixelType *))) == NULL)
	{
		free(buf1D);
		return false;
	}
	// fill (picsizeY) elem = 0 for Y
	memset(&buf1D[0], 0, picsizeY * sizeof(PixelType));
	// fill (picsizeUV * 2) elem = 128 for U, V
	memset(&buf1D[picsizeY], 128, picsizeUV * 2 * sizeof(PixelType));//Owieczka set it

	Y = buf2D;
	U = &(buf2D[height]);
	V = &(buf2D[height + heightUV]);

	// fill Y, U, V from buf1D
	for (h = pos = 0; h < height; h++, pos += width)
		Y[h] = &(buf1D[pos]);
	for (h = 0; h < heightUV; h++, pos += widthUV)
		U[h] = &(buf1D[pos]);
	for (h = 0; h < heightUV; h++, pos += widthUV)
		V[h] = &(buf1D[pos]);

	m_comp[0] = Y;
	m_comp[1] = U;
	m_comp[2] = V;

	m_buffer1D = Y[0]; // buf1D;

	return true;
}

template <class PixelType>
void ImageData<PixelType>::release_mem()
{
	if (Y != NULL)
	{
		//if (Y[0] != NULL) free(Y[0]);
		//free(Y);
		Y = U = V = NULL;
		m_comp[0] = m_comp[1] = m_comp[2] = NULL;
		m_buffer1D = NULL;
	}
}

template <class PixelType>
PixelType* ImageData<PixelType>::getFrame()
{
	return Y[0];
}



template <class PixelType>
bool ImageData<PixelType>::setImage444DataToBGR(ImageData *src)
{
	int h, w, cH, cW;
	int ir, ig, ib;

	if (sampling != 444 || Y == NULL || height != src->getHeight() || width != src->getWidth()) return false;

	switch (src->getSampling())
	{
	case 400:
		for (size_t i = 0; i < 3; i++)
		{
			memcpy(m_comp[i][0], src->Y[0], picsizeY);
		}
	case 420:
		for (h = cH = 0; h < height; h++)
		{
			cH = h >> 1;
			if ((cH << 1) >= h) cH--; //owieczka
			for (w = cW = 0; w < width; w++, cW++)
			{
				for (size_t i = 0; i < 2; i++)
				{
					ib = (int)(src->Y[h][w] + 1.772*(src->U[cH][cW] - 127) + 0.5);
					ig = (int)(src->Y[h][w] - 0.344*(src->U[cH][cW] - 127) - 0.714*(src->V[cH][cW] - 127) + 0.5);
					ir = (int)(src->Y[h][w] + 1.402*(src->V[cH][cW] - 127) + 0.5);
					
					m_comp[0][h][w] = ImageTools::CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
					m_comp[1][h][w] = ImageTools::CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
					m_comp[2][h][w] = ImageTools::CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);

					if (i == 0) w++;
				}
			}
		}
		break;
	case 422:
		for (h = 0; h < height; h++)
		{
			for (w = cW = 0; w < width; w++, cW++)
			{
				for (size_t i = 0; i < 2; i++)
				{
					ib = (int)(src->Y[h][w] + 1.772*(src->U[h][cW] - 127) + 0.5);
					ig = (int)(src->Y[h][w] - 0.344*(src->U[h][cW] - 127) - 0.714*(src->V[h][cW] - 127) + 0.5);
					ir = (int)(src->Y[h][w] + 1.402*(src->V[h][cW] - 127) + 0.5);

					m_comp[0][h][w] = ImageTools::CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
					m_comp[1][h][w] = ImageTools::CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
					m_comp[2][h][w] = ImageTools::CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);

					if (i == 0) w++;
				}
			}
		}
		break;
	case 444:
		for (h = 0; h < height; h++)
		{
			for (w = 0; w < width; w++)
			{
				ib = (int)(src->Y[h][w] + 1.772*(src->U[h][w] - 127) + 0.5);
				ig = (int)(src->Y[h][w] - 0.344*(src->U[h][w] - 127) - 0.714*(src->V[h][w] - 127) + 0.5);
				ir = (int)(src->Y[h][w] + 1.402*(src->V[h][w] - 127) + 0.5);
				m_comp[0][h][w] = ImageTools::CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
				m_comp[1][h][w] = ImageTools::CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
				m_comp[2][h][w] = ImageTools::CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	default:
		memset(m_comp[0][0], 0, m_sizeByte);
		break;
	}

	return true;
}

template <class PixelType>
bool ImageData<PixelType>::setImage444DataToYUV(ImageData *yuvSrc)
{
	int h, dstH, dstW, srcH, srcW;

	if (sampling != 444 || Y == NULL || height != yuvSrc->getHeight() || width != yuvSrc->getWidth()) return false;

	memcpy(Y[0], yuvSrc->Y[0], picsizeY);

	switch (yuvSrc->getSampling())
	{
	case 400:
		memset(U[0], 128, picsizeUV * 2);
		break;
	case 420:
		for (srcH = dstH = 0; dstH < height; dstH += 2, srcH++)
		{
			for (srcW = dstW = 0; dstW < width; dstW += 2, srcW++)
			{
				U[dstH][dstW] = U[dstH + 1][dstW] = U[dstH][dstW + 1] = U[dstH + 1][dstW + 1] = yuvSrc->U[srcH][srcW];
				V[dstH][dstW] = V[dstH + 1][dstW] = V[dstH][dstW + 1] = V[dstH + 1][dstW + 1] = yuvSrc->V[srcH][srcW];
			}
		}
		break;
	case 422:
		for (h = 0; h < height; h++)
		{
			for (srcW = dstW = 0; dstW < width; dstW += 2, srcW++)
			{
				U[h][dstW] = U[h][dstW + 1] = yuvSrc->U[h][srcW];
				V[h][dstW] = V[h][dstW + 1] = yuvSrc->V[h][srcW];
			}
		}
		break;
	case 444:
		memcpy(U[0], yuvSrc->U[0], picsizeUV * 2);
		break;
	}

	return true;
}

template <class PixelType>
void ImageData<PixelType>::setDataFromImgIplBGR(IplImage *imgBGR)
{
	int h, w;
	int fr, fg, fb;
	int iy, iu, iv;
	unsigned char *bufBGR;

	switch (sampling)
	{
	case 400:
		for (h = 0; h < height; h++)
		{
			bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
			for (w = 0; w < width; w++)
			{
				fb = *bufBGR++;	//B
				fg = *bufBGR++;	//G
				fr = *bufBGR++;	//R
				iy = (int)(0.299 * fr + 0.587 * fg + 0.114 * fb + 0.5);
				Y[h][w] = ImageTools::CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	case 420:
		for (h = 0; h < height; h++)
		{
			bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
			for (w = 0; w < width; w++)
			{
				fb = *bufBGR++;	//B
				fg = *bufBGR++;	//G
				fr = *bufBGR++;	//R
				iy = (int)(0.299 * fr + 0.587 * fg + 0.114 * fb + 0.5);
				Y[h][w] = ImageTools::CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
				if (h % 2 == 0 && w % 2 == 0)
				{
					iu = (int)(-0.169 * fr - 0.331 * fg + 0.500 * fb + 127.5);
					iv = (int)(0.500 * fr - 0.419 * fg - 0.081 * fb + 127.5);
					U[h / 2][w / 2] = ImageTools::CLIP3(iu, 0, MaxTypeValue<PixelType>() - 1);
					V[h / 2][w / 2] = ImageTools::CLIP3(iv, 0, MaxTypeValue<PixelType>() - 1);
				}
			}
		}
		break;
	case 422:
		for (h = 0; h < height; h++)
		{
			bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
			for (w = 0; w < width; w++)
			{
				fb = *bufBGR++;	//B
				fg = *bufBGR++;	//G
				fr = *bufBGR++;	//R
				iy = (int)(0.299 * fr + 0.587 * fg + 0.114 * fb + 0.5);
				Y[h][w] = ImageTools::CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
				if (w % 2 == 0)
				{
					iu = (int)(-0.169 * fr - 0.331 * fg + 0.500 * fb + 127.5);
					iv = (int)(0.500 * fr - 0.419 * fg - 0.081 * fb + 127.5);
					U[h][w / 2] = ImageTools::CLIP3(iu, 0, MaxTypeValue<PixelType>() - 1);
					V[h][w / 2] = ImageTools::CLIP3(iv, 0, MaxTypeValue<PixelType>() - 1);
				}
			}
		}
		break;
	case 444:
		for (h = 0; h < height; h++)
		{
			bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
			for (w = 0; w < width; w++)
			{
				fb = *bufBGR++;	//B
				fg = *bufBGR++;	//G
				fr = *bufBGR++;	//R
				iy = (int)(0.299 * fr + 0.587 * fg + 0.114 * fb + 0.5);
				iu = (int)(-0.169 * fr - 0.331 * fg + 0.500 * fb + 127.5);
				iv = (int)(0.500 * fr - 0.419 * fg - 0.081 * fb + 127.5);
				Y[h][w] = ImageTools::CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
				U[h][w] = ImageTools::CLIP3(iu, 0, MaxTypeValue<PixelType>() - 1);
				V[h][w] = ImageTools::CLIP3(iv, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	}
}

template <class PixelType>
void ImageData<PixelType>::setDataFromImgIplYUV(IplImage *imgYUV)
{
	int h, w;
	int iu, iv;
	unsigned char *bufYUV1, *bufYUV2;

	switch (sampling)
	{
	case 400:
		for (h = 0; h < height; h++)
		{
			bufYUV1 = (unsigned char *)&(imgYUV->imageData[h*imgYUV->widthStep]);
			for (w = 0; w < width; w++)
			{
				Y[h][w] = *bufYUV1;	// Y
				bufYUV1 += 3;
			}
		}
		break;
	case 420:
		for (h = 0; h < height; h += 2)
		{
			bufYUV1 = (unsigned char *)&(imgYUV->imageData[h   *imgYUV->widthStep]);
			bufYUV2 = (unsigned char *)&(imgYUV->imageData[(h + 1)*imgYUV->widthStep]);
			for (w = 0; w < width; w += 2)
			{
				Y[h][w] = *bufYUV1++;
				iu = *bufYUV1++;
				iv = *bufYUV1++;

				Y[h + 1][w] = *bufYUV2++;
				iu += *bufYUV2++;
				iv += *bufYUV2++;

				Y[h][w + 1] = *bufYUV1++;
				iu += *bufYUV1++;
				iv += *bufYUV1++;

				Y[h + 1][w + 1] = *bufYUV2++;
				iu += *bufYUV2++;
				iv += *bufYUV2++;

				U[h / 2][w / 2] = ImageTools::CLIP3((iu + 2) / 4, 0, MaxTypeValue<PixelType>() - 1);
				V[h / 2][w / 2] = ImageTools::CLIP3((iv + 2) / 4, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	case 422:
		for (h = 0; h < height; h++)
		{
			bufYUV1 = (unsigned char *)&(imgYUV->imageData[h*imgYUV->widthStep]);
			for (w = 0; w < width; w += 2)
			{
				Y[h][w] = *bufYUV1++;
				iu = *bufYUV1++;
				iv = *bufYUV1++;

				Y[h][w + 1] = *bufYUV1++;
				iu += *bufYUV1++;
				iv += *bufYUV1++;

				U[h][w / 2] = ImageTools::CLIP3((iu + 1) / 2, 0, MaxTypeValue<PixelType>() - 1);
				V[h][w / 2] = ImageTools::CLIP3((iv + 1) / 2, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	case 444:
		for (h = 0; h < height; h++)
		{
			bufYUV1 = (unsigned char *)&(imgYUV->imageData[h*imgYUV->widthStep]);
			for (w = 0; w < width; w++)
			{
				Y[h][w] = *bufYUV1++;
				U[h][w] = *bufYUV1++;
				V[h][w] = *bufYUV1++;
			}
		}
		break;
	}
}

template <class PixelType>
bool ImageData<PixelType>::setUpsampleFilter(unsigned int filterConfig, unsigned int precision)
{
	switch (precision)
	{
		//todo precision 1 not working!
	case 1: // INTEGER PEL
		filter = new FilterDummy2D<PixelType>();
		break;
	case 2: // HALF PEL
		switch (filterConfig)
		{
		case 0: // BI-LINEAR
			filter = new FilterHorizontalLinearHalf2D<PixelType>();
			break;
		case 1: // BI-CUBIC
			filter = new FilterHorizontalCubicHalf2D<PixelType>();
			break;
		case 2: // AVC
			filter = new FilterHorizontalAVCHalf2D<PixelType>();
			break;
		default:
			return false;
			break;
		}
		break;
	case 4: // QUARTER PEL
		switch (filterConfig)
		{
		case 0: // BI-LINEAR
			filter = new FilterHorizontalLinearQuarter2D<PixelType>();
			break;
		case 1: // BI-CUBIC
			filter = new FilterHorizontalCubicQuarter2D<PixelType>();
			break;
		case 2: // AVC
			filter = new FilterHorizontalAVCQuarter2D<PixelType>();
			break;
		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}
	return true;
}

template <class PixelType>
bool ImageData<PixelType>::upsampling(ImageData<PixelType> src, int padding_size)
{
	// need to check buffer size

	filter->apply(src.Y, Y, src.getWidth(), src.getHeight(), padding_size);
	filter->apply(src.U, U, src.getWidthUV(), src.getHeightUV(), padding_size);
	filter->apply(src.V, V, src.getWidthUV(), src.getHeightUV(), padding_size);

	return true;
}

#endif