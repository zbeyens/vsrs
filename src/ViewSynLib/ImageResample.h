#ifndef __INCLUDE_UPSAMPLE_H__
#define __INCLUDE_UPSAMPLE_H__



#ifndef CLIP
#define CLIP(x,min,max) ( (x)<(min)?(min):((x)>(max)?(max):(x)) )
#endif

/*
 * TODO: More s/w work to encapsulate the above functions into the following class
 */

template <class PixelType>
class ImageResample
{
private:

	// Algorithm 0: Simple repeat /drop pixels
	void DropPixel444to420(PixelType* oPic, PixelType* iPic, int Width, int Height);

	void DoUpsample(PixelType* Yo, PixelType* Yi, int Width, int Height);
	void DoDownsample(PixelType* Yo, PixelType* Yi, int Width, int Height);

public:
	ImageResample() {};
	~ImageResample() {};

	// For UV format conversion
	void PictureResample444to420(PixelType* oPic, PixelType* iPic, int Width, int Height);

	// For picture upsample and downsample
	int UpsampleView(PixelType* Yo, PixelType* Yi, int Width, int Height, int Factor);
	int DownsampleView(PixelType* Yo, PixelType* Yi, int Width, int Height, int Factor);
};

/* ************************************************
 *  class ImageResample
 * *********************************************** */


/*
 * \brief
 *    Resampling by simple pixel dropping
 *
 * \input
 * \param Org
 *    Input image (One component, U or V) in 2Widthx2Height
 * \param Width, Height
 *    The resolution of the picture (UV component)
 *
 * \output
 * \param Dst
 *    Output image (One component, U or V) in WidthxHeight
 */
template <class PixelType>
void ImageResample<PixelType>::DropPixel444to420(PixelType* Dst, PixelType* Org, int Width, int Height)
{
	int x, y;
	int w, h;

	w = Width << 1;
	h = Height << 1;

	for (y = 0; y < Height; y++)
		for (x = 0; x < Width; x++)
		{
			Dst[x + y * Width] = Org[x * 2 + y * 2 * w];
		}
}


/*
 * \brief
 *    Resampling from 444 to 420. 2Widthx2Height => WidthxHeight
 *
 * \input
 * \param Org
 *    Input image (One component, U or V) in 2Widthx2Height
 * \param Width, Height
 *    The resolution of the picture (UV component)
 *
 * \output
 * \param Dst
 *    Output image (One component, U or V) in WidthxHeight
 */
template <class PixelType>
void ImageResample<PixelType>::PictureResample444to420(PixelType *Dst, PixelType *Org, int Width, int Height)
{
	DropPixel444to420(Dst, Org, Width, Height);
}

/*
 * \brief
 *    Upsample the picture, and we will get more warped pixels in the synthesized picture
 *
 * \input
 * \param Yi
 *    The input image.  Resolution is Width, Height
 * \param Width, Height
 *    The resolution of the input picture
 *
 * \output
 * \param Yo
 *    The output image. Resolution is Width*2, Height
 *
 * \return
 *    None
 */
template <class PixelType>
void ImageResample<PixelType>::DoUpsample(PixelType* Yo, PixelType* Yi, int Width, int Height)
{
	int i, j, pel[6];
	int WidthMinus1;
	PixelType* out;
	PixelType* in;

	WidthMinus1 = Width - 1;

	for (j = 0; j < Height; j++)
	{
		out = &Yo[j*Width * 2];
		in = &Yi[j*Width];
		for (i = 0; i < Width; i++)
		{
			pel[0] = CLIP(i - 2, 0, WidthMinus1);
			pel[1] = CLIP(i - 1, 0, WidthMinus1);
			pel[2] = CLIP(i, 0, WidthMinus1);
			pel[3] = CLIP(i + 1, 0, WidthMinus1);
			pel[4] = CLIP(i + 2, 0, WidthMinus1);
			pel[5] = CLIP(i + 3, 0, WidthMinus1);

			out[(i) << 1] = in[pel[2]];
			out[((i) << 1) + 1] = CLIP((20 * (in[pel[2]] + in[pel[3]]) - 5 * (in[pel[1]] + in[pel[4]]) + (in[pel[0]] + in[pel[5]]) + 16) >> 5, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}

/*
 * \brief
 *    Downsample the picture, and we will get more warped pixels in the synthesized picture
 *
 * \input
 * \param Yi
 *    The input image.  Resolution is Width, Height
 * \param Width, Height
 *    The resolution of the input picture
 *
 * \output
 * \param Yo
 *    The output image. Resolution is Width*2, Height
 *
 * \return
 *    None
 */
template <class PixelType>
void ImageResample<PixelType>::DoDownsample(PixelType* Yo, PixelType* Yi, int Width, int Height)
{
	int i, j, pel[6];
	int WidthMinus1;
	PixelType* out;
	PixelType* in;

	WidthMinus1 = Width - 1;

	for (j = 0; j < Height; j++)
	{
		out = &Yo[j*Width / 2];
		in = &Yi[j*Width];
		for (i = 0; i < Width; i += 2)
		{
			pel[0] = CLIP(i - 1, 0, WidthMinus1);
			pel[1] = CLIP(i, 0, WidthMinus1);
			pel[2] = CLIP(i + 1, 0, WidthMinus1);

			out[(i >> 1)] = CLIP((in[pel[0]] + 2 * in[pel[1]] + in[pel[2]]) >> 2, 0, MaxTypeValue<PixelType>() - 1);
		}
	}
}

/*
 * \brief
 *    Upsample the picture, and we will get more warped pixels in the synthesized picture
 *
 * \input
 * \param Yi
 *    The input image.  Resolution is Width, Height
 * \param Width, Height
 *    The resolution of the input picture
 * \param Factor
 *    Must be 2 or 4. Upsample to 2 times or 4 times
 *
 * \output
 * \param Yo
 *    The output image. Resolution is Width*2, Height
 *
 * \return
 *    0: success
 *    -1: fail
 */
template <class PixelType>
int ImageResample<PixelType>::UpsampleView(PixelType* Yo, PixelType* Yi, int Width, int Height, int Factor)
{
	PixelType* Ym;

	if (Factor == 2)
		DoUpsample(Yo, Yi, Width, Height);
	else
	{
		Ym = (PixelType*)malloc(Width * 2 * Height * sizeof(PixelType));
		if (Ym == NULL)
		{
			printf("No mem for upsample ref view.\n");
			return -1;
		}
		DoUpsample(Ym, Yi, Width, Height);
		DoUpsample(Yo, Ym, Width * 2, Height);
		free(Ym);
	}

	return 0;
}

/*
 * \brief
 *    Downsample the picture, and we will get more warped pixels in the synthesized picture
 *
 * \input
 * \param Yi
 *    The input image.  Resolution is Width, Height
 * \param Width, Height
 *    The resolution of the input picture
 * \param Factor
 *    Must be 2 or 4. Downsample to 2 times or 4 times
 *
 * \output
 * \param Yo
 *    The output image. Resolution is Width*2, Height
 *
 * \return
 *    None
 */
template <class PixelType>
int ImageResample<PixelType>::DownsampleView(PixelType* Yo, PixelType* Yi, int Width, int Height, int Factor)
{
	PixelType* Ym;

	if (Factor == 2)
		DoDownsample(Yo, Yi, Width, Height);
	else
	{
		Ym = (unsigned char*)malloc(Width / 2 * Height * sizeof(unsigned char));
		if (Ym == NULL)
		{
			printf("No mem for downsample ref view.\n");
			return -1;
		}
		DoDownsample(Ym, Yi, Width, Height);
		DoDownsample(Yo, Ym, Width / 2, Height);
		free(Ym);
	}

	return 0;
}

#endif