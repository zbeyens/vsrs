#include "BoundaryNoiseRemoval3D.h"

BoundaryNoiseRemoval3D::BoundaryNoiseRemoval3D()
{
	m_precision = 1;
}

BoundaryNoiseRemoval3D::~BoundaryNoiseRemoval3D()
{
}

void BoundaryNoiseRemoval3D::calcWeight()
{
	if (cfg.getViewBlending() == cfg.BLEND_CLOSER) { // different results !
		if (m_LeftBaseLineDistance <= m_RightBaseLineDistance) {
			m_weightLeft = 1.0;
			m_weightRight = 0.0;
		}
		else {
			m_weightLeft = 0.0;
			m_weightRight = 1.0;
		}
	}
	else if (cfg.getViewBlending() == cfg.BLEND_ALL) {
		if (m_LeftBaseLineDistance <= m_RightBaseLineDistance) {
			m_weightLeft = fabs(1.0 - (fabs(m_LeftBaseLineDistance) / (fabs(m_LeftBaseLineDistance) + fabs(m_RightBaseLineDistance))));
			m_weightRight = fabs(1.0 - (fabs(m_RightBaseLineDistance) / (fabs(m_LeftBaseLineDistance) + fabs(m_RightBaseLineDistance))));
		}
		else {
			m_weightLeft = fabs(1.0 - (fabs(m_RightBaseLineDistance) / (fabs(m_LeftBaseLineDistance) + fabs(m_RightBaseLineDistance))));
			m_weightRight = fabs(1.0 - (fabs(m_LeftBaseLineDistance) / (fabs(m_LeftBaseLineDistance) + fabs(m_RightBaseLineDistance))));
		}
	}
}

void BoundaryNoiseRemoval3D::Blending(Image<ImageType>* pLeft, Image<ImageType>* pRight, unique_ptr<Image<ImageType>>& pSyn, bool SynthesisMode)
{
	Image<ImageType> temp1, temp2;
	IplImage *TempImage;
	ImageResample<ImageType> Resampling;
	ImageType* LeftBuffer, *RightBuffer, *SrcBuffer, *DstBuffer;

	temp1.resizeYUV(m_height, m_width, 444);
	DstBuffer = temp1.getBuffer1D();
	LeftBuffer = pLeft->getBuffer1D();
	RightBuffer = pRight->getBuffer1D();

	for (int i = 0; i < m_width*m_height * 3; i++) {
		DstBuffer[i] = (ImageType)guard((double)m_weightLeft* (double)LeftBuffer[i] + (double)m_weightRight*(double)RightBuffer[i], 0, MaxTypeValue<ImageType>() - 1);
	}
	TempImage = cvCreateImage(cvSize(m_width, m_height), 8, 3);
	if (cfg.getColorSpace() == cfg.COLOR_SPACE_RGB) {   // BGR
		memcpy(TempImage->imageData, temp1.getBuffer1D(), m_width*m_height * 3);
		pSyn->convertMatBGRToYUV(TempImage);
	}
	else if (cfg.getColorSpace() == cfg.COLOR_SPACE_YUV) {              // YUV
		memcpy(TempImage->imageData, temp1.getBuffer1D(), m_width*m_height * 3);
		pSyn->convertMatYUVToYUV(TempImage);
	}
}

void BoundaryNoiseRemoval3D::RemainingHoleFilling(Image<ImageType>* pSrc)
{
	int i, j, tWidth, tHeight, CountHole;
	bool isValidLeft, isValidRight, isValid_Y, isValid_U, isValid_V, isComHole;
	ImageType* buffer;
	tWidth = pSrc->getWidth();
	tHeight = pSrc->getHeight();
	buffer = pSrc->getBuffer1D();

	for (j = 0; j < tHeight; j++) {
		for (i = 0; i < tWidth; i++) {
			buffer[j*tWidth * 3 + i * 3 + 0] ? isValid_Y = true : isValid_Y = false;
			buffer[j*tWidth * 3 + i * 3 + 1] ? isValid_U = true : isValid_U = false;
			buffer[j*tWidth * 3 + i * 3 + 2] ? isValid_V = true : isValid_V = false;
			m_imgCommonHole->imageData[j*tWidth + i] ? isComHole = true : isComHole = false;
			if (!isValid_Y || !isValid_U || !isValid_V || isComHole) {
				buffer[j*tWidth * 3 + i * 3 + 0] = 0;
				buffer[j*tWidth * 3 + i * 3 + 1] = 0;
				buffer[j*tWidth * 3 + i * 3 + 2] = 0;
			}
		}
	}

	CountHole = 0;
	for (j = 0; j < tHeight; j++) {
		for (i = 0; i < tWidth; i++) {
			if (buffer[j*tWidth * 3 + i * 3] == 0) {
				CountHole++;
			}
		}
	}

	// may have a huge loop here if too much holes.
	while (CountHole) {
		for (j = 0; j < tHeight; j++) {
			for (i = 0; i < tWidth; i++) {
				if (i == 0 && buffer[j*tWidth * 3 + i * 3 + 0] == 0) {
					isValidLeft = false;
					buffer[j*tWidth * 3 + (i + 1) * 3 + 0] && buffer[j*tWidth * 3 + (i + 1) * 3 + 1] && buffer[j*tWidth * 3 + (i + 1) * 3 + 2] ? isValidRight = true : isValidRight = false;

					if (!isValidLeft && isValidRight) {
						buffer[j*tWidth * 3 + i * 3 + 0] = buffer[j*tWidth * 3 + (i + 1) * 3 + 0];
						buffer[j*tWidth * 3 + i * 3 + 1] = buffer[j*tWidth * 3 + (i + 1) * 3 + 1];
						buffer[j*tWidth * 3 + i * 3 + 2] = buffer[j*tWidth * 3 + (i + 1) * 3 + 2];
						CountHole--;
					}
				}
				else if (i == tWidth - 1 && buffer[j*tWidth * 3 + i * 3 + 0] == 0) {
					buffer[j*tWidth * 3 + (i - 1) * 3 + 0] && buffer[j*tWidth * 3 + (i - 1) * 3 + 1] && buffer[j*tWidth * 3 + (i - 1) * 3 + 2] ? isValidLeft = true : isValidLeft = false;
					isValidRight = false;

					if (isValidLeft && !isValidRight) {
						buffer[j*tWidth * 3 + i * 3 + 0] = buffer[j*tWidth * 3 + (i - 1) * 3 + 0];
						buffer[j*tWidth * 3 + i * 3 + 1] = buffer[j*tWidth * 3 + (i - 1) * 3 + 1];
						buffer[j*tWidth * 3 + i * 3 + 2] = buffer[j*tWidth * 3 + (i - 1) * 3 + 2];
						CountHole--;
					}
				}
				else if (buffer[j*tWidth * 3 + i * 3 + 0] == 0) {
					buffer[j*tWidth * 3 + (i - 1) * 3 + 0] ? isValidLeft = true : isValidLeft = false;
					buffer[j*tWidth * 3 + (i + 1) * 3 + 0] ? isValidRight = true : isValidRight = false;
					if (isValidLeft && isValidRight) {
						buffer[j*tWidth * 3 + i * 3 + 0] = guard((buffer[j*tWidth * 3 + (i - 1) * 3 + 0] + buffer[j*tWidth * 3 + (i + 1) * 3 + 0]) / 2 + 0.5, 0, MaxTypeValue<ImageType>() - 1);
						buffer[j*tWidth * 3 + i * 3 + 1] = guard((buffer[j*tWidth * 3 + (i - 1) * 3 + 1] + buffer[j*tWidth * 3 + (i + 1) * 3 + 1]) / 2 + 0.5, 0, MaxTypeValue<ImageType>() - 1);
						buffer[j*tWidth * 3 + i * 3 + 2] = guard((buffer[j*tWidth * 3 + (i - 1) * 3 + 2] + buffer[j*tWidth * 3 + (i + 1) * 3 + 2]) / 2 + 0.5, 0, MaxTypeValue<ImageType>() - 1);
						CountHole--;
					}
					else if (isValidLeft && !isValidRight) {
						buffer[j*tWidth * 3 + i * 3 + 0] = buffer[j*tWidth * 3 + (i - 1) * 3 + 0];
						buffer[j*tWidth * 3 + i * 3 + 1] = buffer[j*tWidth * 3 + (i - 1) * 3 + 1];
						buffer[j*tWidth * 3 + i * 3 + 2] = buffer[j*tWidth * 3 + (i - 1) * 3 + 2];
						CountHole--;
					}
					else if (!isValidLeft && isValidRight) {
						buffer[j*tWidth * 3 + i * 3 + 0] = buffer[j*tWidth * 3 + (i + 1) * 3 + 0];
						buffer[j*tWidth * 3 + i * 3 + 1] = buffer[j*tWidth * 3 + (i + 1) * 3 + 1];
						buffer[j*tWidth * 3 + i * 3 + 2] = buffer[j*tWidth * 3 + (i + 1) * 3 + 2];
						CountHole--;
					}
				}
			}
		}
	}
}

void BoundaryNoiseRemoval3D::HoleFillingWithExpandedHole(Image<ImageType>* pSrc, Image<ImageType>* pTar, IplImage * m_imgExpandedHole, bool SynthesisMode)
{
	int i, j, tWidth, tHeight;
	BYTE* Src_buffer, *Tar_buffer;

	Src_buffer = pSrc->getBuffer1D();
	Tar_buffer = pTar->getBuffer1D();

	for (j = 0; j < m_height; j++) {
		for (i = 0; i < m_width; i++) {
			if ((uchar)m_imgExpandedHole->imageData[j*m_width + i] == MAX_HOLE - 1) {
				Tar_buffer[j*m_width * 3 + i * 3 + 0] = Src_buffer[j*m_width * 3 + i * 3 + 0];
				Tar_buffer[j*m_width * 3 + i * 3 + 1] = Src_buffer[j*m_width * 3 + i * 3 + 1];
				Tar_buffer[j*m_width * 3 + i * 3 + 2] = Src_buffer[j*m_width * 3 + i * 3 + 2];
				m_imgExpandedHole->imageData[j*m_width + i] = 0;
			}
		}
	}
}
