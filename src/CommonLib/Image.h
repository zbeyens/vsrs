#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "ImageResample.h"
#include "ImageTools.h"
#include "Tools.h"

/**
	Class containing the pixel values in a YUV or BGR format
*/
template <class PixelType>
class Image
{
public:
	Image();
	Image(int h, int w, int chroma_format);
	~Image();

	/**
		Set the size of the YUV components, depending on the chroma format.
		Allocate YUV components.
		@param w Image width
		@param h Image height
		@param chroma_format	YUV format
	*/
	bool initYUV(int h, int w, int chroma_format);

	/**
		Allocate Y, U, V and set image data inside.
		@param widthStep	width of each row
	*/
	bool initYUVFromMat(int widthStep);

	/**
		Allocate Y and set image data inside.
	*/
	bool initYFromMat();

	/**
		Creates an image header and allocates the image data
		@param w Image width
		@param h Image height
		@param channels Number of channels per pixel. Creates images with interleaved channels.
	*/
	void initMat(int w, int h, int channels) {
		m_imageMat = cvCreateImage(cvSize(w, h), IMAGE_DEPTH * sizeof(PixelType), channels);
	}

	/**
		Clear the image data.
	*/
	void clearMat() {
		cvZero(m_imageMat);
	}

	/**
		Convert the mat in YUVXXX to YUV444 format. Used at the beginning of the pipeline.
		@param src	image in YUVXXX format to convert into this image
	*/
	bool convertYUVToYUV444(unique_ptr<Image<PixelType>> &src);
	void convertYUV400ToYUV444(unique_ptr<Image<PixelType>> &src);
	void convertYUV420ToYUV444(unique_ptr<Image<PixelType>> &src);
	void convertYUV422ToYUV444(unique_ptr<Image<PixelType>> &src);
	void convertYUV444ToYUV444(unique_ptr<Image<PixelType>> &src);

	/**
		Convert the mat in YUVXXX to BGR format. Used at the beginning of the pipeline.
		@param src	image in YUVXXX format to convert into this image
	*/
	bool convertYUVToBGR(unique_ptr<Image<PixelType>> &src);
	void convertYUV400ToBGR(unique_ptr<Image<PixelType>> &src);
	void convertYUV420ToBGR(unique_ptr<Image<PixelType>> &src);
	void convertYUV422ToBGR(unique_ptr<Image<PixelType>> &src);
	void convertYUV444ToBGR(unique_ptr<Image<PixelType>> &src);

	/**
		Convert the mat in YUV format to YUVXXX format. Used at the end of the pipeline.
		@param imgYUV	the mat to convert
	*/
	void convertMatYUVToYUV(IplImage *imgYUV);
	void convertMatYUVToYUV400(IplImage *imgYUV);
	void convertMatYUVToYUV420(IplImage *imgYUV);
	void convertMatYUVToYUV422(IplImage *imgYUV);
	void convertMatYUVToYUV444(IplImage *imgYUV);

	/**
		Convert the mat in BGR format to YUVXXX format. Used at the end of the pipeline.
		@param imgBGR	the mat to convert
	*/
	void convertMatBGRToYUV(IplImage *imgBGR);
	void convertMatBGRToYUV400(IplImage *imgBGR);
	void convertMatBGRToYUV420(IplImage *imgBGR);
	void convertMatBGRToYUV422(IplImage *imgBGR);
	void convertMatBGRToYUV444(IplImage *imgBGR);

	/**
		Convert the mat of img src to buffer1D. Used for the 1D pipeline.
		@param src	src with a mat to convert
	*/
	void convertMatToBuffer1D(Image<PixelType>* src, int channels);

	int getSampling() { return m_sampling; }
	int getHeight() { return m_height; }
	int getWidth() { return m_width; }
	int getHeightUV() { return m_heightUV; }
	int getWidthUV() { return m_widthUV; }
	int	getSize() { return m_sizeByte; }
	PixelType* getFrame() { return Y[0]; }
	PixelType***    getData() { return m_comp; }
	PixelType*      getBuffer1D() { return m_buffer1D; }
	PixelType** getY() { return Y; }
	PixelType** getU() { return U; }
	PixelType** getV() { return V; }
	IplImage* getMat() { return m_imageMat; }
	PixelType getMatData(int index) { return m_imageMat->imageData[index]; }

	void setMatData(int index, PixelType value) { m_imageMat->imageData[index] = value; }

private:
	void prealloc();
	/**
		Allocate comp (Y, U, V) and the 1D buffer
	*/
	bool      allocate_mem();
	void      release_mem();

	ConfigSyn& cfg = ConfigSyn::getInstance();
	int m_height;		//!> height of the image
	int m_width;		//!> width of the image
	int m_heightUV;		//!< height of U and V depending on chroma format
	int m_widthUV;		//!< width of U and V depending on chroma format
	int m_picsizeY;		//!< pic size for Y: height*width
	int m_picsizeUV;		//!< pic size for each U and V: heightUV*widthUV
	int m_sizeByte;		//!< YUV size in bytes: picsizeY + picsizeUV * 2
	int m_sampling;		//!< chroma format: 444, 422, 420 or 400

	PixelType **Y;	//!> Y component of the image
	PixelType **U;	//!> U component of the image
	PixelType **V;	//!> V component of the image

	PixelType      **m_comp[3];	//!> YUV or BGR data depending on ColorSpace

	PixelType      *m_buffer1D;  //!> 1D array containing the Y, U and V component

	IplImage * m_imageMat;	//!> the openCV matrix for mathematical operations
};

template <class PixelType>
Image<PixelType>::Image()
{
	prealloc();
}

template <class PixelType>
Image<PixelType>::Image(int h, int w, int chroma_format)
{
	prealloc();
	initYUV(h, w, chroma_format);
}

template <class PixelType>
Image<PixelType>::~Image()
{
	release_mem();
}

template <class PixelType>
void Image<PixelType>::prealloc()
{
	Y = U = V = NULL;
	m_comp[0] = m_comp[1] = m_comp[2] = NULL;
	m_buffer1D = NULL;
	m_imageMat = NULL;
}

template <class PixelType>
bool Image<PixelType>::initYUV(int h, int w, int chroma_format)
{
	m_height = h;
	m_width = w;
	m_sampling = chroma_format;

	switch (m_sampling)
	{
	case 400: // padding chroma components
		m_heightUV = 0;
		m_widthUV = 0;
		break;
	case 420:
		m_heightUV = m_height / 2;
		m_widthUV = m_width / 2;
		break;
	case 422:
		m_heightUV = m_height;
		m_widthUV = m_width / 2;
		break;
	case 444:
		m_heightUV = m_height;
		m_widthUV = m_width;
		break;
	default:
		fprintf(stderr, "Unknown chroma sampling\n");
		Y = U = V = NULL;
		return false;
	}

	m_picsizeY = m_height * m_width;
	m_picsizeUV = m_heightUV * m_widthUV;
	m_sizeByte = (m_picsizeY + m_picsizeUV * 2) * sizeof(PixelType);

	return allocate_mem();
}

template <class PixelType>
bool Image<PixelType>::allocate_mem()
{
	int h, pos;
	// sizeByte elem
	PixelType *buf1D;

	// (height + heightUV * 2) rows, Y (0) - U (height) - V (height + heightUV)
	PixelType **buf2D;

	release_mem();

	if ((buf1D = (PixelType *)malloc(m_sizeByte)) == NULL) return false;

	if ((buf2D = (PixelType **)malloc((m_height + m_heightUV * 2) * sizeof(PixelType *))) == NULL)
	{
		free(buf1D);
		return false;
	}
	// fill (picsizeY) elem = 0 for Y
	memset(&buf1D[0], 0, m_picsizeY * sizeof(PixelType));
	// fill (picsizeUV * 2) elem = 128 for U, V
	memset(&buf1D[m_picsizeY], 128, m_picsizeUV * 2 * sizeof(PixelType));//Owieczka set it

	Y = buf2D;
	U = &(buf2D[m_height]);
	V = &(buf2D[m_height + m_heightUV]);

	// fill Y, U, V from buf1D
	for (h = pos = 0; h < m_height; h++, pos += m_width)
		Y[h] = &(buf1D[pos]);
	for (h = 0; h < m_heightUV; h++, pos += m_widthUV)
		U[h] = &(buf1D[pos]);
	for (h = 0; h < m_heightUV; h++, pos += m_widthUV)
		V[h] = &(buf1D[pos]);

	m_comp[0] = Y;
	m_comp[1] = U;
	m_comp[2] = V;

	m_buffer1D = Y[0]; // buf1D;

	return true;
}

template <class PixelType>
void Image<PixelType>::release_mem()
{
	//Tools::safeFree(Y);
	//Tools::safeFree(U);
	//Tools::safeFree(V);

	Tools::safeReleaseImage(m_imageMat);

	m_comp[0] = m_comp[1] = m_comp[2] = NULL;
	m_buffer1D = NULL;
}

template <class PixelType>
bool Image<PixelType>::initYUVFromMat(int widthStep)
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	if ((Y = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL
		|| (U = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL
		|| (V = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL) return false;

	for (int h = 0; h < sh; h++)
	{
		if ((Y[h] = (PixelType *)malloc(widthStep * sizeof(PixelType))) == NULL
			|| (U[h] = (PixelType *)malloc(widthStep * sizeof(PixelType))) == NULL
			|| (V[h] = (PixelType *)malloc(widthStep * sizeof(PixelType))) == NULL) return false;
	}

	//store imageY, U, V - format image->imageData: [YUVYUVYUV...]
	for (int h = 0; h < sh; h++)
	{
		for (uint w = 0; w < sw; w++)
		{
			Y[h][w] = (PixelType)(m_imageMat->imageData[(h*sw + w) * 3]);
			U[h][w] = (PixelType)(m_imageMat->imageData[(h*sw + w) * 3 + 1]);
			V[h][w] = (PixelType)(m_imageMat->imageData[(h*sw + w) * 3 + 2]);
		}
	}

	return true;
}

template <class PixelType>
bool Image<PixelType>::initYFromMat()
{
	int sh = cfg.getSourceHeight();

	if ((Y = (PixelType **)malloc(sh * sizeof(PixelType *))) == NULL) return false;

	//h	= position in height
	//pos = position in pixel, increment line width
	uint h, pos;
	for (h = pos = 0; h < sh; h++, pos += m_imageMat->widthStep)
	{
		Y[h] = (PixelType *) &(m_imageMat->imageData[pos]);
	}

	return true;
}

template <class PixelType>
bool Image<PixelType>::convertYUVToBGR(unique_ptr<Image>& src)
{
	if (m_sampling != 444 || Y == NULL || m_height != src->getHeight() || m_width != src->getWidth()) return false;

	switch (src->getSampling())
	{
	case 400:
		convertYUV400ToBGR(src);
		break;
	case 420:
		convertYUV420ToBGR(src);
		break;
	case 422:
		convertYUV422ToBGR(src);
		break;
	case 444:
		convertYUV444ToBGR(src);
		break;
	default:
		memset(m_comp[0][0], 0, m_sizeByte);
		break;
	}

	return true;
}

template <class PixelType>
void Image<PixelType>::convertYUV400ToBGR(unique_ptr<Image>& src)
{
	for (size_t i = 0; i < 3; i++)
	{
		memcpy(m_comp[i][0], src->Y[0], m_picsizeY);
	}
}

template <class PixelType>
void Image<PixelType>::convertYUV420ToBGR(unique_ptr<Image>& src)
{
	int h, w, cH, cW;
	int ir, ig, ib;
	for (h = cH = 0; h < m_height; h++)
	{
		cH = h >> 1;
		if ((cH << 1) >= h) cH--; //owieczka
		for (w = cW = 0; w < m_width; w++, cW++)
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
}

template <class PixelType>
void Image<PixelType>::convertYUV422ToBGR(unique_ptr<Image>& src)
{
	int h, w, cH, cW;
	int ir, ig, ib;
	for (h = 0; h < m_height; h++)
	{
		for (w = cW = 0; w < m_width; w++, cW++)
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
}

template <class PixelType>
void Image<PixelType>::convertYUV444ToBGR(unique_ptr<Image>& src)
{
	int h, w, cH, cW;
	int ir, ig, ib;
	for (h = 0; h < m_height; h++)
	{
		for (w = 0; w < m_width; w++)
		{
			ib = (int)(src->Y[h][w] + 1.772*(src->U[h][w] - 127) + 0.5);
			ig = (int)(src->Y[h][w] - 0.344*(src->U[h][w] - 127) - 0.714*(src->V[h][w] - 127) + 0.5);
			ir = (int)(src->Y[h][w] + 1.402*(src->V[h][w] - 127) + 0.5);
			m_comp[0][h][w] = ImageTools::CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
			m_comp[1][h][w] = ImageTools::CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
			m_comp[2][h][w] = ImageTools::CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}

template <class PixelType>
bool Image<PixelType>::convertYUVToYUV444(unique_ptr<Image> &src)
{
	if (m_sampling != 444 || Y == NULL || m_height != src->getHeight() || m_width != src->getWidth()) return false;

	memcpy(Y[0], src->Y[0], m_picsizeY);

	switch (src->getSampling())
	{
	case 400:
		convertYUV400ToYUV444(src);
		break;
	case 420:
		convertYUV420ToYUV444(src);
		break;
	case 422:
		convertYUV422ToYUV444(src);
		break;
	case 444:
		convertYUV444ToYUV444(src);
		break;
	}

	return true;
}

template <class PixelType>
void Image<PixelType>::convertYUV400ToYUV444(unique_ptr<Image> &src)
{
	memset(U[0], 128, m_picsizeUV * 2);
}

template <class PixelType>
void Image<PixelType>::convertYUV420ToYUV444(unique_ptr<Image> &src)
{
	int h, dstH, dstW, srcH, srcW;

	for (srcH = dstH = 0; dstH < m_height; dstH += 2, srcH++)
	{
		for (srcW = dstW = 0; dstW < m_width; dstW += 2, srcW++)
		{
			U[dstH][dstW] = U[dstH + 1][dstW] = U[dstH][dstW + 1] = U[dstH + 1][dstW + 1] = src->U[srcH][srcW];
			V[dstH][dstW] = V[dstH + 1][dstW] = V[dstH][dstW + 1] = V[dstH + 1][dstW + 1] = src->V[srcH][srcW];
		}
	}
}

template <class PixelType>
void Image<PixelType>::convertYUV422ToYUV444(unique_ptr<Image> &src)
{
	int h, dstH, dstW, srcH, srcW;

	for (h = 0; h < m_height; h++)
	{
		for (srcW = dstW = 0; dstW < m_width; dstW += 2, srcW++)
		{
			U[h][dstW] = U[h][dstW + 1] = src->U[h][srcW];
			V[h][dstW] = V[h][dstW + 1] = src->V[h][srcW];
		}
	}
}

template <class PixelType>
void Image<PixelType>::convertYUV444ToYUV444(unique_ptr<Image> &src)
{
	memcpy(U[0], src->U[0], m_picsizeUV * 2);
}

template <class PixelType>
void Image<PixelType>::convertMatBGRToYUV(IplImage *imgBGR)
{
	switch (m_sampling)
	{
	case 400:
		convertMatBGRToYUV400(imgBGR);
		break;
	case 420:
		convertMatBGRToYUV420(imgBGR);
		break;
	case 422:
		convertMatBGRToYUV422(imgBGR);
		break;
	case 444:
		convertMatBGRToYUV444(imgBGR);
		break;
	}
}

template <class PixelType>
void Image<PixelType>::convertMatBGRToYUV400(IplImage *imgBGR)
{
	int h, w;
	int fr, fg, fb;
	int iy, iu, iv;
	unsigned char *bufBGR;
	for (h = 0; h < m_height; h++)
	{
		bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
		for (w = 0; w < m_width; w++)
		{
			fb = *bufBGR++;	//B
			fg = *bufBGR++;	//G
			fr = *bufBGR++;	//R
			iy = (int)(0.299 * fr + 0.587 * fg + 0.114 * fb + 0.5);
			Y[h][w] = ImageTools::CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}

template <class PixelType>
void Image<PixelType>::convertMatBGRToYUV420(IplImage *imgBGR)
{
	int h, w;
	int fr, fg, fb;
	int iy, iu, iv;
	unsigned char *bufBGR;
	for (h = 0; h < m_height; h++)
	{
		bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
		for (w = 0; w < m_width; w++)
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
}

template <class PixelType>
void Image<PixelType>::convertMatBGRToYUV422(IplImage *imgBGR)
{
	int h, w;
	int fr, fg, fb;
	int iy, iu, iv;
	unsigned char *bufBGR;
	for (h = 0; h < m_height; h++)
	{
		bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
		for (w = 0; w < m_width; w++)
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
}

template <class PixelType>
void Image<PixelType>::convertMatBGRToYUV444(IplImage *imgBGR)
{
	int h, w;
	int fr, fg, fb;
	int iy, iu, iv;
	unsigned char *bufBGR;
	for (h = 0; h < m_height; h++)
	{
		bufBGR = (unsigned char *)&(imgBGR->imageData[h*imgBGR->widthStep]);
		for (w = 0; w < m_width; w++)
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
}

template <class PixelType>
void Image<PixelType>::convertMatYUVToYUV(IplImage *imgYUV)
{
	switch (m_sampling)
	{
	case 400:
		convertMatYUVToYUV400(imgYUV);
		break;
	case 420:
		convertMatYUVToYUV420(imgYUV);
		break;
	case 422:
		convertMatYUVToYUV422(imgYUV);
		break;
	case 444:
		convertMatYUVToYUV444(imgYUV);
		break;
	}
}

template <class PixelType>
void Image<PixelType>::convertMatYUVToYUV400(IplImage *imgYUV)
{
	int h, w;
	unsigned char *bufYUV1;
	for (h = 0; h < m_height; h++)
	{
		bufYUV1 = (unsigned char *)&(imgYUV->imageData[h*imgYUV->widthStep]);
		for (w = 0; w < m_width; w++)
		{
			Y[h][w] = *bufYUV1;	// Y
			bufYUV1 += 3;
		}
	}
}

template <class PixelType>
void Image<PixelType>::convertMatYUVToYUV420(IplImage *imgYUV)
{
	int h, w;
	int iu, iv;
	unsigned char *bufYUV1, *bufYUV2;
	for (h = 0; h < m_height; h += 2)
	{
		bufYUV1 = (unsigned char *)&(imgYUV->imageData[h   *imgYUV->widthStep]);
		bufYUV2 = (unsigned char *)&(imgYUV->imageData[(h + 1)*imgYUV->widthStep]);
		for (w = 0; w < m_width; w += 2)
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
}

template <class PixelType>
void Image<PixelType>::convertMatYUVToYUV422(IplImage *imgYUV)
{
	int h, w;
	int iu, iv;
	unsigned char *bufYUV1;
	for (h = 0; h < m_height; h++)
	{
		bufYUV1 = (unsigned char *)&(imgYUV->imageData[h*imgYUV->widthStep]);
		for (w = 0; w < m_width; w += 2)
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
}

template <class PixelType>
void Image<PixelType>::convertMatYUVToYUV444(IplImage *imgYUV)
{
	int h, w;
	unsigned char *bufYUV1;
	for (h = 0; h < m_height; h++)
	{
		bufYUV1 = (unsigned char *)&(imgYUV->imageData[h*imgYUV->widthStep]);
		for (w = 0; w < m_width; w++)
		{
			Y[h][w] = *bufYUV1++;
			U[h][w] = *bufYUV1++;
			V[h][w] = *bufYUV1++;
		}
	}
}

template <class PixelType>
void Image<PixelType>::convertMatToBuffer1D(Image<PixelType>* src, int channels)
{
	cvCopy(src->getMat(), m_imageMat);
	memcpy(m_buffer1D, m_imageMat->imageData, cfg.getSourceHeight() * cfg.getSourceWidth() * sizeof(PixelType) * channels);
}
