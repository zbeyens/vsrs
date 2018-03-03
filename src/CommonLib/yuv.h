/***********************************************************

			 YUV image input/output functions

 ***********************************************************/
#ifndef _INCLUDE_YUV_H_
#define _INCLUDE_YUV_H_

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "upsample.h"

template <class PixelType>
class ImageYuv
{
public:
	ImageYuv();
	ImageYuv(int h, int w, int chroma_format);
	virtual ~ImageYuv();

	bool      resize(int h, int w, int chroma_format);
	bool      readOneFrame(FILE *fp, int frameno = -1);
	bool      writeOneFrame(FILE *fp);

	void      setDataFromImgBGR(IplImage *imgBGR);
	void      setDataFromImgYUV(IplImage *imgYUV);

	bool      setData444_inIBGR(ImageYuv<PixelType> *yuvSrc);
	bool      setData444_inIYUV(ImageYuv<PixelType> *yuvSrc);

	bool      setUpsampleFilter(unsigned int filter, unsigned int precision);
	bool      upsampling(ImageYuv<PixelType> *src, int padding_size = 0);

	int        getSampling() { return sampling; }
	int        getHeight() { return height; }
	int        getWidth() { return width; }
	int        getHeightUV() { return heightUV; }
	int        getWidthUV() { return widthUV; }
	PixelType***    getData() { return m_yuv; }
	PixelType*      getBuffer() { return m_buffer1D; }

	PixelType **Y;
	PixelType **U;
	PixelType **V;

private:
	void prealloc();
	bool      allocate_mem();
	void      release_mem();

	ConfigSyn& cfg = ConfigSyn::getInstance();
	int height;
	int width;
	int heightUV;		//!< height of U and V depending on chroma format
	int widthUV;		//!< width of U and V depending on chroma format
	int picsizeY;		//!< pic size for Y: height*width
	int picsizeUV;		//!< pic size for each U and V: heightUV*widthUV
	int size_in_byte;	//!< YUV size: picsizeY + picsizeUV * 2
	int sampling;		//!< chroma format: 444, 422, 420 or 400

	//upsampling filter function depending on the cfg.filter and cfg.precision
	void(*FuncFilter)(PixelType **in, PixelType **out, int width, int height, int padding_size);

	//Y, U, V
	PixelType      **m_yuv[3];

	PixelType      *m_buffer1D;  //!> 1D array for the Y, U and V component.
};

template <class PixelType>
ImageYuv<PixelType>::ImageYuv()
{
	prealloc();
}

template <class PixelType>
ImageYuv<PixelType>::ImageYuv(int h, int w, int chroma_format)
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
ImageYuv<PixelType>::~ImageYuv()
{
	release_mem();
}

template <class PixelType>
void ImageYuv<PixelType>::prealloc()
{
	Y = U = V = NULL;
	m_yuv[0] = m_yuv[1] = m_yuv[2] = NULL;
	m_buffer1D = NULL;
	FuncFilter = NULL;
}

template <class PixelType>
bool ImageYuv<PixelType>::resize(int h, int w, int chroma_format)
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
	size_in_byte = (picsizeY + picsizeUV * 2) * sizeof(PixelType);

	return allocate_mem();
}

/**
	malloc comp (Y, U, V) and the 1D buffer
*/
template <class PixelType>
bool ImageYuv<PixelType>::allocate_mem()
{
	int h, pos;
	// size_in_byte elem
	PixelType *buf1D;

	// (height + heightUV * 2) rows, Y (0) - U (height) - V (height + heightUV)
	PixelType **buf2D;

	release_mem();

	if ((buf1D = (PixelType *)malloc(size_in_byte)) == NULL) return false;

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

	m_yuv[0] = Y;
	m_yuv[1] = U;
	m_yuv[2] = V;

	m_buffer1D = Y[0]; // buf1D;

	return true;
}

template <class PixelType>
void ImageYuv<PixelType>::release_mem()
{
	if (Y != NULL)
	{
		if (Y[0] != NULL) free(Y[0]);
		free(Y);
		Y = U = V = NULL;
		m_yuv[0] = m_yuv[1] = m_yuv[2] = NULL;
		m_buffer1D = NULL;
	}
}

template <class PixelType>
bool ImageYuv<PixelType>::readOneFrame(FILE *fp, int frameno)
{
	if (Y == NULL || fp == NULL || m_buffer1D == NULL) return false;

	if (frameno != -1)
	{
		long offs = long(size_in_byte)*long(frameno);
		fseek(fp, offs, SEEK_SET);
	}

	if (fread(Y[0], size_in_byte, 1, fp) != 1)
	{
		fprintf(stderr, "EOF\n");
		return false;
	}

	return true;
}

template <class PixelType>
bool ImageYuv<PixelType>::writeOneFrame(FILE *fp)
{
	if (Y == NULL || fp == NULL || m_buffer1D == NULL) return false;

	if (fwrite(Y[0], size_in_byte, 1, fp) != 1)
	{
		fprintf(stderr, "EOF\n");
		return false;
	}

	return true;
}

template <class PixelType>
void ImageYuv<PixelType>::setDataFromImgBGR(IplImage *imgBGR)
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
				Y[h][w] = CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
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
				Y[h][w] = CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
				if (h % 2 == 0 && w % 2 == 0)
				{
					iu = (int)(-0.169 * fr - 0.331 * fg + 0.500 * fb + 127.5);
					iv = (int)(0.500 * fr - 0.419 * fg - 0.081 * fb + 127.5);
					U[h / 2][w / 2] = CLIP3(iu, 0, MaxTypeValue<PixelType>() - 1);
					V[h / 2][w / 2] = CLIP3(iv, 0, MaxTypeValue<PixelType>() - 1);
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
				Y[h][w] = CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
				if (w % 2 == 0)
				{
					iu = (int)(-0.169 * fr - 0.331 * fg + 0.500 * fb + 127.5);
					iv = (int)(0.500 * fr - 0.419 * fg - 0.081 * fb + 127.5);
					U[h][w / 2] = CLIP3(iu, 0, MaxTypeValue<PixelType>() - 1);
					V[h][w / 2] = CLIP3(iv, 0, MaxTypeValue<PixelType>() - 1);
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
				Y[h][w] = CLIP3(iy, 0, MaxTypeValue<PixelType>() - 1);
				U[h][w] = CLIP3(iu, 0, MaxTypeValue<PixelType>() - 1);
				V[h][w] = CLIP3(iv, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	}
}

template <class PixelType>
bool ImageYuv<PixelType>::setData444_inIBGR(ImageYuv *yuvSrc)
{
	int h, w, cH, cW;
	int ir, ig, ib;

	if (sampling != 444 || Y == NULL || height != yuvSrc->getHeight() || width != yuvSrc->getWidth()) return false;

	switch (yuvSrc->getSampling())
	{
	case 400:
		memcpy(m_yuv[0][0], yuvSrc->Y[0], picsizeY);
		memcpy(m_yuv[1][0], yuvSrc->Y[0], picsizeY);
		memcpy(m_yuv[2][0], yuvSrc->Y[0], picsizeY);
	case 420:
		for (h = cH = 0; h < height; h++)
		{
			cH = h >> 1;
			if ((cH << 1) >= h) cH--; //owieczka
			for (w = cW = 0; w < width; w++, cW++)
			{
				ib = (int)(yuvSrc->Y[h][w] + 1.772*(yuvSrc->U[cH][cW] - 127) + 0.5);
				ig = (int)(yuvSrc->Y[h][w] - 0.344*(yuvSrc->U[cH][cW] - 127) - 0.714*(yuvSrc->V[cH][cW] - 127) + 0.5);
				ir = (int)(yuvSrc->Y[h][w] + 1.402*(yuvSrc->V[cH][cW] - 127) + 0.5);
				m_yuv[0][h][w] = CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[1][h][w] = CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[2][h][w] = CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);
				w++;
				ib = (int)(yuvSrc->Y[h][w] + 1.772*(yuvSrc->U[cH][cW] - 127) + 0.5);
				ig = (int)(yuvSrc->Y[h][w] - 0.344*(yuvSrc->U[cH][cW] - 127) - 0.714*(yuvSrc->V[cH][cW] - 127) + 0.5);
				ir = (int)(yuvSrc->Y[h][w] + 1.402*(yuvSrc->V[cH][cW] - 127) + 0.5);
				m_yuv[0][h][w] = CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[1][h][w] = CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[2][h][w] = CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	case 422:
		for (h = 0; h < height; h++)
		{
			for (w = cW = 0; w < width; w++, cW++)
			{
				ib = (int)(yuvSrc->Y[h][w] + 1.772*(yuvSrc->U[h][cW] - 127) + 0.5);
				ig = (int)(yuvSrc->Y[h][w] - 0.344*(yuvSrc->U[h][cW] - 127) - 0.714*(yuvSrc->V[h][cW] - 127) + 0.5);
				ir = (int)(yuvSrc->Y[h][w] + 1.402*(yuvSrc->V[h][cW] - 127) + 0.5);
				m_yuv[0][h][w] = CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[1][h][w] = CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[2][h][w] = CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);
				w++;
				ib = (int)(yuvSrc->Y[h][w] + 1.772*(yuvSrc->U[h][cW] - 127) + 0.5);
				ig = (int)(yuvSrc->Y[h][w] - 0.344*(yuvSrc->U[h][cW] - 127) - 0.714*(yuvSrc->V[h][cW] - 127) + 0.5);
				ir = (int)(yuvSrc->Y[h][w] + 1.402*(yuvSrc->V[h][cW] - 127) + 0.5);
				m_yuv[0][h][w] = CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[1][h][w] = CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[2][h][w] = CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	case 444:
		for (h = 0; h < height; h++)
		{
			for (w = 0; w < width; w++)
			{
				ib = (int)(yuvSrc->Y[h][w] + 1.772*(yuvSrc->U[h][w] - 127) + 0.5);
				ig = (int)(yuvSrc->Y[h][w] - 0.344*(yuvSrc->U[h][w] - 127) - 0.714*(yuvSrc->V[h][w] - 127) + 0.5);
				ir = (int)(yuvSrc->Y[h][w] + 1.402*(yuvSrc->V[h][w] - 127) + 0.5);
				m_yuv[0][h][w] = CLIP3(ib, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[1][h][w] = CLIP3(ig, 0, MaxTypeValue<PixelType>() - 1);
				m_yuv[2][h][w] = CLIP3(ir, 0, MaxTypeValue<PixelType>() - 1);
			}
		}
		break;
	default:
		memset(m_yuv[0][0], 0, size_in_byte);
		break;
	}

	return true;
}

template <class PixelType>
bool ImageYuv<PixelType>::setData444_inIYUV(ImageYuv *yuvSrc)
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
void ImageYuv<PixelType>::setDataFromImgYUV(IplImage *imgYUV)
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

				U[h / 2][w / 2] = CLIP3((iu + 2) / 4, 0, MaxTypeValue<PixelType>() - 1);
				V[h / 2][w / 2] = CLIP3((iv + 2) / 4, 0, MaxTypeValue<PixelType>() - 1);
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

				U[h][w / 2] = CLIP3((iu + 1) / 2, 0, MaxTypeValue<PixelType>() - 1);
				V[h][w / 2] = CLIP3((iv + 1) / 2, 0, MaxTypeValue<PixelType>() - 1);
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
bool ImageYuv<PixelType>::setUpsampleFilter(unsigned int filter, unsigned int precision)
{
	switch (precision)
	{
		//todo precision 1 not working!
	case 1: // INTEGER PEL
		FuncFilter = DummyFilter_2D;
		break;
	case 2: // HALF PEL
		switch (filter)
		{
		case 0: // BI-LINEAR
			FuncFilter = HorizontalLinearFilter_2D_half;
			break;
		case 1: // BI-CUBIC
			FuncFilter = HorizontalCubicFilter_2D_half;
			break;
		case 2: // AVC
			FuncFilter = HorizontalAVCFilter_2D_half;
			break;
		default:
			return false;
			break;
		}
		break;
	case 4: // QUARTER PEL
		switch (filter)
		{
		case 0: // BI-LINEAR
			FuncFilter = HorizontalLinearFilter_2D_qpel;
			break;
		case 1: // BI-CUBIC
			FuncFilter = HorizontalCubicFilter_2D_qpel;
			break;
		case 2: // AVC
			FuncFilter = HorizontalAVCFilter_2D_qpel;
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
bool ImageYuv<PixelType>::upsampling(ImageYuv<PixelType> *src, int padding_size)
{
	// need to check buffer size

	FuncFilter(src->Y, Y, src->getWidth(), src->getHeight(), padding_size);
	FuncFilter(src->U, U, src->getWidthUV(), src->getHeightUV(), padding_size);
	FuncFilter(src->V, V, src->getWidthUV(), src->getHeightUV(), padding_size);

	return true;
}

#endif