#include "BoundaryNoiseRemoval1D.h"

BoundaryNoiseRemoval1D::BoundaryNoiseRemoval1D()
{
	m_precision = cfg.getPrecision();
}

BoundaryNoiseRemoval1D::~BoundaryNoiseRemoval1D()
{
}

void BoundaryNoiseRemoval1D::calcWeight()
{
	if (LTranslation[LEFTVIEW] <= LTranslation[RGHTVIEW]) {
		m_weightLeft = fabs(1.0 - (fabs(LTranslation[LEFTVIEW]) / (fabs(LTranslation[LEFTVIEW]) + fabs(LTranslation[RGHTVIEW]))));
		m_weightRight = fabs(1.0 - (fabs(LTranslation[RGHTVIEW]) / (fabs(LTranslation[LEFTVIEW]) + fabs(LTranslation[RGHTVIEW]))));
	}
	else {
		m_weightLeft = fabs(1.0 - (fabs(LTranslation[RGHTVIEW]) / (fabs(LTranslation[LEFTVIEW]) + fabs(LTranslation[RGHTVIEW]))));
		m_weightRight = fabs(1.0 - (fabs(LTranslation[LEFTVIEW]) / (fabs(LTranslation[LEFTVIEW]) + fabs(LTranslation[RGHTVIEW]))));
	}
}

void BoundaryNoiseRemoval1D::Blending(Image<ImageType>* pLeft, Image<ImageType>* pRight, unique_ptr<Image<ImageType>>& pSyn, bool SynthesisMode)
{
	Image<ImageType> temp1, temp2;
	IplImage *TempImage;
	ImageResample<ImageType> Resampling;
	ImageType* LeftBuffer, *RightBuffer, *SrcBuffer, *DstBuffer;

	int tWidth, tHeight;
	ImageType *Src, *Dst;
	tWidth = m_width * m_precision;
	tHeight = m_height;
	temp1.resizeYUV(tHeight, tWidth, 444);
	temp2.resizeYUV(m_height, m_width, 444);

	// Blending
	LeftBuffer = pLeft->getBuffer1D();
	RightBuffer = pRight->getBuffer1D();
	Dst = temp1.getBuffer1D();
	for (int i = 0; i < tWidth*tHeight * 3; i++) {
		Dst[i] = (ImageType)guard((m_weightLeft* LeftBuffer[i] + m_weightRight * RightBuffer[i] + 0.5), 0, MaxTypeValue<ImageType>() - 1);
	}

	// DownSampling
	SrcBuffer = temp1.getBuffer1D();
	Dst = temp2.getBuffer1D();

	if (m_precision == 1) {
		memcpy(Dst, SrcBuffer, tWidth*tHeight * 3);
	}
	else {   // Half-pel, Quarter-pel
		Resampling.DownsampleView(&Dst[m_width*m_height * 0], &SrcBuffer[tWidth*tHeight * 0], tWidth, m_height, m_precision);  // Y
		Resampling.DownsampleView(&Dst[m_width*m_height * 1], &SrcBuffer[tWidth*tHeight * 1], tWidth, m_height, m_precision);  // U
		Resampling.DownsampleView(&Dst[m_width*m_height * 2], &SrcBuffer[tWidth*tHeight * 2], tWidth, m_height, m_precision);  // V
	}

	SrcBuffer = temp2.getBuffer1D();
	Dst = pSyn->getBuffer1D();
	memcpy(Dst, SrcBuffer, m_width*m_height);
	Dst = &Dst[m_width*m_height];
	SrcBuffer = &SrcBuffer[m_width*m_height];
	for (int j = 0; j < m_height / 2; j++) {
		for (int i = 0; i < m_width / 2; i++) {
			Dst[j*m_width / 2 + i] = SrcBuffer[(j * 2)*m_width + (i * 2)];
		}
	}
	Dst = &Dst[m_width*m_height / 4];
	SrcBuffer = &SrcBuffer[m_width*m_height];
	for (int j = 0; j < m_height / 2; j++) {
		for (int i = 0; i < m_width / 2; i++) {
			Dst[j*m_width / 2 + i] = SrcBuffer[(j * 2)*m_width + (i * 2)];
		}
	}
}

void BoundaryNoiseRemoval1D::RemainingHoleFilling(Image<ImageType>* pSrc)
{
	int i, j, tWidth, tHeight, CountHole;
	bool isValidLeft, isValidRight, isValid_Y, isValid_U, isValid_V;
	ImageType* buffer;
	tWidth = pSrc->getWidth();
	tHeight = pSrc->getHeight();
	buffer = pSrc->getBuffer1D();

	for (j = 0; j < tHeight; j++) {
		for (i = 0; i < tWidth; i++) {
			buffer[j*tWidth * 3 + i * 3 + 0] ? isValid_Y = true : isValid_Y = false;
			buffer[j*tWidth * 3 + i * 3 + 1] ? isValid_U = true : isValid_U = false;
			buffer[j*tWidth * 3 + i * 3 + 2] ? isValid_V = true : isValid_V = false;
			if (!isValid_Y || !isValid_U || !isValid_V) {
				buffer[j*tWidth * 3 + i * 3 + 0] = 0;
				buffer[j*tWidth * 3 + i * 3 + 1] = 0;
				buffer[j*tWidth * 3 + i * 3 + 2] = 0;
			}
		}
	}

	CountHole = 0;
	for (j = 0; j < tHeight; j++) {
		for (i = 0; i < tWidth; i++) {
			if (buffer[j*tWidth + i] == 0) {
				CountHole++;
			}
		}
	}

	while (CountHole) {
		for (j = 0; j < tHeight; j++) {
			for (i = 0; i < tWidth; i++) {
				if (i == 0 && buffer[tWidth*tHeight * 0 + j * tWidth + i] == 0) {
					isValidLeft = false;
					buffer[tWidth*tHeight * 0 + j * tWidth + i + 1] ? isValidRight = true : isValidRight = false;

					if (!isValidLeft && isValidRight) {
						buffer[tWidth*tHeight * 0 + j * tWidth + i] = buffer[tWidth*tHeight * 0 + j * tWidth + i + 1];
						buffer[tWidth*tHeight * 1 + j * tWidth + i] = buffer[tWidth*tHeight * 1 + j * tWidth + i + 1];
						buffer[tWidth*tHeight * 2 + j * tWidth + i] = buffer[tWidth*tHeight * 2 + j * tWidth + i + 1];
						CountHole--;
					}
				}
				else if (i == tWidth - 1 && buffer[tWidth*tHeight * 0 + j * tWidth + i] == 0) {
					buffer[tWidth*tHeight * 0 + j * tWidth + i - 1] ? isValidLeft = true : isValidLeft = false;
					isValidRight = false;

					if (isValidLeft && !isValidRight) {
						buffer[tWidth*tHeight * 0 + j * tWidth + i] = buffer[tWidth*tHeight * 0 + j * tWidth + i - 1];
						buffer[tWidth*tHeight * 1 + j * tWidth + i] = buffer[tWidth*tHeight * 1 + j * tWidth + i - 1];
						buffer[tWidth*tHeight * 2 + j * tWidth + i] = buffer[tWidth*tHeight * 2 + j * tWidth + i - 1];
						CountHole--;
					}
				}
				else if (buffer[tWidth*tHeight * 0 + j * tWidth + i] == 0) {
					buffer[tWidth*tHeight * 0 + j * tWidth + i - 1] ? isValidLeft = true : isValidLeft = false;
					buffer[tWidth*tHeight * 0 + j * tWidth + i + 1] ? isValidRight = true : isValidRight = false;

					if (isValidLeft && isValidRight) {
						buffer[tWidth*tHeight * 0 + j * tWidth + i] = guard((buffer[tWidth*tHeight * 0 + j * tWidth + i - 1] + buffer[tWidth*tHeight * 0 + j * tWidth + i + 1]) / 2 + 0.5, 0, MaxTypeValue<ImageType>() - 1);
						buffer[tWidth*tHeight * 1 + j * tWidth + i] = guard((buffer[tWidth*tHeight * 1 + j * tWidth + i - 1] + buffer[tWidth*tHeight * 1 + j * tWidth + i + 1]) / 2 + 0.5, 0, MaxTypeValue<ImageType>() - 1);
						buffer[tWidth*tHeight * 2 + j * tWidth + i] = guard((buffer[tWidth*tHeight * 2 + j * tWidth + i - 1] + buffer[tWidth*tHeight * 2 + j * tWidth + i + 1]) / 2 + 0.5, 0, MaxTypeValue<ImageType>() - 1);
						CountHole--;
					}
					else if (isValidLeft && !isValidRight) {
						buffer[tWidth*tHeight * 0 + j * tWidth + i] = buffer[tWidth*tHeight * 0 + j * tWidth + i - 1];
						buffer[tWidth*tHeight * 1 + j * tWidth + i] = buffer[tWidth*tHeight * 1 + j * tWidth + i - 1];
						buffer[tWidth*tHeight * 2 + j * tWidth + i] = buffer[tWidth*tHeight * 2 + j * tWidth + i - 1];
						CountHole--;
					}
					else if (!isValidLeft && isValidRight) {
						buffer[tWidth*tHeight * 0 + j * tWidth + i] = buffer[tWidth*tHeight * 0 + j * tWidth + i + 1];
						buffer[tWidth*tHeight * 1 + j * tWidth + i] = buffer[tWidth*tHeight * 1 + j * tWidth + i + 1];
						buffer[tWidth*tHeight * 2 + j * tWidth + i] = buffer[tWidth*tHeight * 2 + j * tWidth + i + 1];
						CountHole--;
					}
				}
			}
		}
	}
}

void BoundaryNoiseRemoval1D::HoleFillingWithExpandedHole(Image<ImageType>* pSrc, Image<ImageType>* pTar, IplImage * m_imgExpandedHole, bool SynthesisMode)
{
	int i, j, tWidth, tHeight;
	BYTE* Src_buffer, *Tar_buffer;

	Src_buffer = pSrc->getBuffer1D();
	Tar_buffer = pTar->getBuffer1D();

	tWidth = m_width * m_precision;
	tHeight = m_height;
	for (j = 0; j < tHeight; j++) {     // Y
		for (i = 0; i < tWidth; i++) {
			if ((uchar)m_imgExpandedHole->imageData[j*tWidth + i] == MAX_HOLE - 1 && (uchar)m_imgCommonHole->imageData[j*tWidth + i] == 0) {
				Tar_buffer[tWidth*tHeight * 0 + j * tWidth + i] = Src_buffer[tWidth*tHeight * 0 + j * tWidth + i];
				Tar_buffer[tWidth*tHeight * 1 + j * tWidth + i] = Src_buffer[tWidth*tHeight * 1 + j * tWidth + i];
				Tar_buffer[tWidth*tHeight * 2 + j * tWidth + i] = Src_buffer[tWidth*tHeight * 2 + j * tWidth + i];
				m_imgExpandedHole->imageData[j*tWidth + i] = 0;
			}
		}
	}
}
