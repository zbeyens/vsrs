#include "Warp.h"

bool Warp::init()
{
	return true;
}

void Warp::erodebound(IplImage * bound, int flag)
{
	int width = bound->width;
	int height = bound->height;
	uchar *ub = (uchar *)bound->imageData;

	if (flag)
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				int l = i + j * width;
				if (ub[l] == 255)
				{
					int ll = l;
					while ((ub[ll] == 255) && (i < width))
					{
						ub[ll] = 0;
						ll++;
						i++;
					}
					ub[ll - 1] = 255;
				}
			}
		}
	}
	else
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				int l = i + j * width;
				if (ub[l] == 255)
				{
					int ll = l;
					while ((ub[ll] == 255) && (i < width))
					{
						ub[ll] = 0;
						ll++;
						i++;
					}
					ub[l] = 255;
				}
			}
		}
	}
}
