#include "ViewSynthesis.h"

ViewSynthesis::ViewSynthesis()
	: cfg(ConfigSyn::getInstance())
{
	m_pViewSynthesisGeneral = NULL;
	m_pViewSynthesis1D = NULL;

	m_pcDepthMapLeft = m_pcDepthMapRight = NULL;
	m_pcImageLeft = m_pcImageRight = NULL;
	m_pcTempYuvLeft = m_pcTempYuvRight = NULL;

	m_pSynColorLeft = NULL;
	m_pSynColorRight = NULL;
	m_pSynDepthLeft = NULL;
	m_pSynDepthRight = NULL;

	m_iFrameNumber = 0; //Zhejiang

#ifdef _DEBUG
	m_ucSetup = 0;
#endif
}

ViewSynthesis::~ViewSynthesis()
{
	Tools::safeDelete(m_pViewSynthesisGeneral);
	Tools::safeDelete(m_pViewSynthesis1D);

	Tools::safeDelete(m_pcDepthMapLeft);
	Tools::safeDelete(m_pcDepthMapRight);
	Tools::safeDelete(m_pcTempYuvLeft);
	Tools::safeDelete(m_pcTempYuvRight);
	Tools::safeDelete(m_pcImageLeft);
	Tools::safeDelete(m_pcImageRight);

	Tools::safeFree(m_pSynColorLeft);
	Tools::safeFree(m_pSynColorRight);
	Tools::safeFree(m_pSynDepthLeft);
	Tools::safeFree(m_pSynDepthRight);

#ifdef _DEBUG
	m_ucSetup = 0;
#endif
}

bool ViewSynthesis::Init()
{
	if (cfg.getSynthesisMode() == 0)  // General mode
	{
		m_pViewSynthesisGeneral = new ViewSynthesisGeneral();

		if (!m_pViewSynthesisGeneral->InitLR()) return false;
	}
	else  // 1D mode
	{
		m_pViewSynthesis1D = new ViewSynthesis1D();
	}

	m_pBoundaryNoiseRemoval = new BoundaryNoiseRemoval(); // Boundary Noise Removal

	mHeight = cfg.getSourceHeight();
	mWidth = cfg.getSourceWidth() * cfg.getPrecision();
	initInputImages();
	initOutputImages();


#ifdef _DEBUG
	m_ucSetup = 1;
#endif

	return true;
}

void ViewSynthesis::initInputImages()
{
	int srcWidth = cfg.getSourceWidth();
	int srcHeight = cfg.getSourceHeight();

	m_pcDepthMapLeft = new ImageYuv<DepthType>(srcHeight, srcWidth, DEPTHMAP_CHROMA_FORMAT);
	m_pcDepthMapRight = new ImageYuv<DepthType>(srcHeight, srcWidth, DEPTHMAP_CHROMA_FORMAT);

	int imageWidth;
	if (cfg.getSynthesisMode() == 0) // General mode
		imageWidth = mWidth;
	else
		imageWidth = srcWidth;

	m_pcImageLeft = new ImageYuv<ImageType>(mHeight, mWidth, IMAGE_CHROMA_FORMAT);
	m_pcImageRight = new ImageYuv<ImageType>(mHeight, mWidth, IMAGE_CHROMA_FORMAT);

	if (cfg.getPrecision() != 1)
	{
		m_pcTempYuvLeft = new ImageYuv<ImageType>(srcHeight, srcWidth, IMAGE_CHROMA_FORMAT);
		m_pcTempYuvRight = new ImageYuv<ImageType>(srcHeight, srcWidth, IMAGE_CHROMA_FORMAT);
	}
	else
	{
		m_pcTempYuvLeft = m_pcImageLeft;
		m_pcTempYuvRight = m_pcImageRight;
	}
}

void ViewSynthesis::initOutputImages()
{
	// Prepare buffer for boundary noise removal:
	m_pSynColorLeft = (unsigned char*)malloc(sizeof(unsigned char)*(mWidth*mHeight * 3)); // in 444
	m_pSynColorRight = (unsigned char*)malloc(sizeof(unsigned char)*(mWidth*mHeight * 3)); // in 444
	m_pSynDepthLeft = (unsigned char*)malloc(sizeof(unsigned char)*(mWidth*mHeight)); // in 400
	m_pSynDepthRight = (unsigned char*)malloc(sizeof(unsigned char)*(mWidth*mHeight)); // in 400
}

bool ViewSynthesis::SetReferenceImage(int iLeft, ImageYuv<ImageType> *pcYuv)
{
#ifdef _DEBUG
	if (m_ucSetup == 0) return false;
#endif

	if (iLeft)
	{
		if (cfg.getColorSpace())
			m_pcTempYuvLeft->setData444_inIBGR(pcYuv); // RGB
		else
			m_pcTempYuvLeft->setData444_inIYUV(pcYuv); // YUV
		m_pcImageLeft->upsampling(m_pcTempYuvLeft);
	}
	else
	{
		if (cfg.getColorSpace())
			m_pcTempYuvRight->setData444_inIBGR(pcYuv); // RGB
		else
			m_pcTempYuvRight->setData444_inIYUV(pcYuv); // YUV
		m_pcImageRight->upsampling(m_pcTempYuvRight);
	}

#ifdef _DEBUG
	m_ucSetup = 1;
#endif

	return true;
}

/*
 * \brief
 * The main interface function to perform view interpolation
 *   to be called from the application
 *
 * \param pSynYuvBuffer
 *   Store the synthesized picture into the buffer
 *
 * \return
 *    true: if succeed;
 *    false: if fails.
 */

bool ViewSynthesis::DoViewInterpolation(ImageYuv<ImageType>* pSynYuvBuffer)
{
	/*
	  ImageType*** RefLeft  = m_pcImageLeft->getData();
	  ImageType*** RefRight = m_pcImageRight->getData();
	  DepthType** RefDepthLeft  = m_pcDepthMapLeft->Y;
	  DepthType** RefDepthRight = m_pcDepthMapRight->Y;
	//*/

	bool ret = false;
	if (cfg.getSynthesisMode() == 0) // General mode
		ret = xViewSynthesisGeneralMode(pSynYuvBuffer);
	else if (cfg.getSynthesisMode() == 1) // 1-D mode
		ret = xViewSynthesis1DMode(pSynYuvBuffer);

	if (ret == false)
		return ret;

	return ret;
}

bool ViewSynthesis::xViewSynthesisGeneralMode(ImageYuv<ImageType>* pSynYuvBuffer)
{
	ImageType*** RefLeft = m_pcImageLeft->getData();
	ImageType*** RefRight = m_pcImageRight->getData();
	DepthType** RefDepthLeft = m_pcDepthMapLeft->Y;
	DepthType** RefDepthRight = m_pcDepthMapRight->Y;

	if (0 != m_pViewSynthesisGeneral->DoOneFrameGeneral(RefLeft, RefRight, RefDepthLeft, RefDepthRight, pSynYuvBuffer))
		return false;

	if (cfg.getBoundaryNoiseRemoval()) {
		ImageYuv<ImageType> pRefLeft;
		ImageYuv<ImageType> pRefRight;
		ImageYuv<DepthType> pRefDepthLeft;
		ImageYuv<DepthType> pRefDepthRight;
		ImageYuv<HoleType> pRefHoleLeft;
		ImageYuv<HoleType> pRefHoleRight;
		int Width = mWidth / cfg.getPrecision();
		m_pBoundaryNoiseRemoval->SetViewBlending(cfg.getViewBlending());
		m_pBoundaryNoiseRemoval->SetColorSpace(cfg.getColorSpace());

		if (!pRefLeft.resize(mHeight, Width, IMAGE_CHROMA_FORMAT))      return false;
		if (!pRefRight.resize(mHeight, Width, IMAGE_CHROMA_FORMAT))     return false;
		if (!pRefDepthLeft.resize(mHeight, Width, DEPTHMAP_CHROMA_FORMAT)) return false;
		if (!pRefDepthRight.resize(mHeight, Width, DEPTHMAP_CHROMA_FORMAT))return false;
		if (!pRefHoleLeft.resize(mHeight, Width, HOLE_CHROMA_FORMAT))  return false;
		if (!pRefHoleRight.resize(mHeight, Width, HOLE_CHROMA_FORMAT)) return false;

		xFileConvertingforGeneralMode(&pRefLeft, &pRefRight, &pRefDepthLeft, &pRefDepthRight, &pRefHoleLeft, &pRefHoleRight);
		xBoundaryNoiseRemoval(&pRefLeft, &pRefRight, &pRefDepthLeft, &pRefDepthRight, &pRefHoleLeft, &pRefHoleRight, pSynYuvBuffer, false/*General Mode*/);
	}

	return true;
}

/*
 * To perform 1-D view interpolation
 *
 * \Output:
 *    All the color images below are in 444, depth images are 400
 * \param pSynYuvBuffer
 *    To store the synthesis result.
 * \member m_pSynColorLeft
 *    To store the synthesis color component from left ref w/o hole filling
 * \member m_pSynColorRight
 *    To store the synthesis color component from right ref w/o hole filling
 * \member m_pSynDepthLeft
 *    To store the synthesis depth component from left ref w/o hole filling
 * \member m_pSynDepthRight
 *    To store the synthesis depth component from right ref w/o hole filling
 *
 * Return:
 *    true:  if succeed;
 *    false: if fails.
 */
bool ViewSynthesis::xViewSynthesis1DMode(ImageYuv<ImageType>* pSynYuvBuffer)
{
	ImageType* RefLeft = m_pcImageLeft->getBuffer();
	ImageType* RefRight = m_pcImageRight->getBuffer();
	DepthType* RefDepthLeft = m_pcDepthMapLeft->getBuffer();
	DepthType* RefDepthRight = m_pcDepthMapRight->getBuffer();

	m_pViewSynthesis1D->SetFrameNumber(getFrameNumber()); //Zhejiang
	if (0 != m_pViewSynthesis1D->DoOneFrame(RefLeft, RefRight, RefDepthLeft, RefDepthRight, pSynYuvBuffer->getBuffer()))
		return false;

	if (cfg.getBoundaryNoiseRemoval()) {
		ImageYuv<ImageType> pRefLeft;
		ImageYuv<ImageType> pRefRight;
		ImageYuv<DepthType> pRefDepthLeft;
		ImageYuv<DepthType> pRefDepthRight;
		ImageYuv<HoleType> pRefHoleLeft;
		ImageYuv<HoleType> pRefHoleRight;
		int Width = m_pViewSynthesis1D->GetWidth();
		int Height = m_pViewSynthesis1D->GetHeight();
		int SampleFactor = m_pViewSynthesis1D->GetSubPelOption() * m_pViewSynthesis1D->GetUpsampleRefs();
		m_pBoundaryNoiseRemoval->SetViewBlending(cfg.getViewBlending());
		m_pBoundaryNoiseRemoval->SetColorSpace(cfg.getColorSpace());
		m_pBoundaryNoiseRemoval->SetPrecision(SampleFactor);

		if (!pRefLeft.resize(Height, Width*SampleFactor, IMAGE_CHROMA_FORMAT)) return false;
		if (!pRefRight.resize(Height, Width*SampleFactor, IMAGE_CHROMA_FORMAT)) return false;
		if (!pRefDepthLeft.resize(Height, Width*SampleFactor, DEPTHMAP_CHROMA_FORMAT)) return false;
		if (!pRefDepthRight.resize(Height, Width*SampleFactor, DEPTHMAP_CHROMA_FORMAT)) return false;
		if (!pRefHoleLeft.resize(Height, Width*SampleFactor, HOLE1D_CHROMA_FORMAT)) return false;
		if (!pRefHoleRight.resize(Height, Width*SampleFactor, HOLE1D_CHROMA_FORMAT)) return false;

		xFileConvertingfor1DMode(&pRefLeft, &pRefRight, &pRefDepthLeft, &pRefDepthRight, &pRefHoleLeft, &pRefHoleRight);
		xBoundaryNoiseRemoval(&pRefLeft, &pRefRight, &pRefDepthLeft, &pRefDepthRight, &pRefHoleLeft, &pRefHoleRight, pSynYuvBuffer, true/*1D Mode*/);
	}

	return true;
}

bool ViewSynthesis::xBoundaryNoiseRemoval(ImageYuv<ImageType>* pRefLeft, ImageYuv<ImageType>* pRefRight, ImageYuv<DepthType>* pRefDepthLeft, ImageYuv<DepthType>* pRefDepthRight, ImageYuv<HoleType>* pRefHoleLeft, ImageYuv<HoleType>* pRefHoleRight, ImageYuv<ImageType>* pSynYuvBuffer, bool SynthesisMode)
{
	switch (SynthesisMode)
	{
	case false:  // General Mode
		m_pBoundaryNoiseRemoval->SetWidth(mWidth / cfg.getPrecision());
		m_pBoundaryNoiseRemoval->SetHeight(mHeight);
		m_pBoundaryNoiseRemoval->SetLeftBaseLineDist(m_pViewSynthesisGeneral->GetInterpolatedLeft()->getCam().getBaselineDistance());
		m_pBoundaryNoiseRemoval->SetRightBaseLineDist(m_pViewSynthesisGeneral->GetInterpolatedRight()->getCam().getBaselineDistance());
		m_pBoundaryNoiseRemoval->SetLeftH_V2R(m_pViewSynthesisGeneral->GetInterpolatedLeft()->GetMatH_V2R());
		m_pBoundaryNoiseRemoval->SetRightH_V2R(m_pViewSynthesisGeneral->GetInterpolatedRight()->GetMatH_V2R());
		break;
	case true:   // 1D Mode
		m_pBoundaryNoiseRemoval->SetWidth(m_pViewSynthesis1D->GetWidth());
		m_pBoundaryNoiseRemoval->SetHeight(m_pViewSynthesis1D->GetHeight());
		m_pBoundaryNoiseRemoval->SetduPrincipal(m_pViewSynthesis1D->GetduPrincipal());
		m_pBoundaryNoiseRemoval->SetFocalLength(m_pViewSynthesis1D->GetFocalLength());
		m_pBoundaryNoiseRemoval->SetLTranslationLeft(m_pViewSynthesis1D->GetLTranslation());
		m_pBoundaryNoiseRemoval->SetZfar(m_pViewSynthesis1D->GetZfar());
		m_pBoundaryNoiseRemoval->SetZnear(m_pViewSynthesis1D->GetZnear());
		break;
	}

	if (0 != m_pBoundaryNoiseRemoval->DoBoundaryNoiseRemoval(pRefLeft, pRefRight, pRefDepthLeft, pRefDepthRight, pRefHoleLeft, pRefHoleRight, pSynYuvBuffer, SynthesisMode))
		return false;

	/*FILE *fp = fopen("mask.yuv", "wb");
	for (j=0;j<m_iHeight*m_iWidth;j++) {
		fputc(tar_L[j], fp);
	}
	fclose(fp);*/

	return true;
}

void ViewSynthesis::xFileConvertingforGeneralMode(ImageYuv<ImageType>* pRefLeft, ImageYuv<ImageType>* pRefRight, ImageYuv<DepthType>* pRefDepthLeft, ImageYuv<DepthType>* pRefDepthRight, ImageYuv<HoleType>* pRefHoleLeft, ImageYuv<HoleType>* pRefHoleRight)
{
	int Width, Height;
	ImageType* tar_L_image, *tar_R_image;
	ImageType* org_L_image, *org_R_image;
	DepthType* tar_L_depth, *tar_R_depth;
	DepthType* org_L_depth, *org_R_depth;
	HoleType* tar_L_hole, *tar_R_hole;
	HoleType* org_L_hole, *org_R_hole;
	Width = mWidth / cfg.getPrecision();
	Height = mHeight;

	// Color Image with hole
	org_L_image = (ImageType*)m_pViewSynthesisGeneral->GetSynLeftWithHole()->imageData;
	org_R_image = (ImageType*)m_pViewSynthesisGeneral->GetSynRightWithHole()->imageData;
	tar_L_image = pRefLeft->getBuffer();
	tar_R_image = pRefRight->getBuffer();
	memcpy(tar_L_image, org_L_image, Height*Width * 3 * sizeof(ImageType));
	memcpy(tar_R_image, org_R_image, Height*Width * 3 * sizeof(ImageType));

	org_L_depth = (DepthType*)m_pViewSynthesisGeneral->GetSynDepthLeftWithHole()->imageData;
	org_R_depth = (DepthType*)m_pViewSynthesisGeneral->GetSynDepthRightWithHole()->imageData;
	tar_L_depth = pRefDepthLeft->getBuffer();
	tar_R_depth = pRefDepthRight->getBuffer();
	memcpy(tar_L_depth, org_L_depth, Height*Width * sizeof(DepthType));
	memcpy(tar_R_depth, org_R_depth, Height*Width * sizeof(DepthType));

	org_L_hole = (HoleType*)m_pViewSynthesisGeneral->GetSynHoleLeft()->imageData;
	org_R_hole = (HoleType*)m_pViewSynthesisGeneral->GetSynHoleRight()->imageData;
	tar_L_hole = pRefHoleLeft->getBuffer();
	tar_R_hole = pRefHoleRight->getBuffer();
	memcpy(tar_L_hole, org_L_hole, Height*Width * sizeof(HoleType));
	memcpy(tar_R_hole, org_R_hole, Height*Width * sizeof(HoleType));
}

void ViewSynthesis::xFileConvertingfor1DMode(ImageYuv<ImageType>* pRefLeft, ImageYuv<ImageType>* pRefRight, ImageYuv<DepthType>* pRefDepthLeft, ImageYuv<DepthType>* pRefDepthRight, ImageYuv<HoleType>* pRefHoleLeft, ImageYuv<HoleType>* pRefHoleRight)
{
	int Width, Height, UpSampleFactor;
	//CPictureResample resmple;
	ImageType* tar_L, *tar_R;
	UpSampleFactor = m_pViewSynthesis1D->GetSubPelOption() * m_pViewSynthesis1D->GetUpsampleRefs();
	Width = m_pViewSynthesis1D->GetWidth()*UpSampleFactor;
	Height = m_pViewSynthesis1D->GetHeight();

	//CIYuv pRefSrc, pRefDst;
	//pRefSrc.resize(Height, Width, 444);
	//pRefDst.resize(Height, Width, 444);

	// Left Synthesized Image
	tar_L = pRefLeft->getBuffer();
	memcpy(&tar_L[Height*Width * 0], m_pViewSynthesis1D->GetSynColorLeftY(), Height*Width);
	memcpy(&tar_L[Height*Width * 1], m_pViewSynthesis1D->GetSynColorLeftU(), Height*Width);
	memcpy(&tar_L[Height*Width * 2], m_pViewSynthesis1D->GetSynColorLeftV(), Height*Width);

	// Right Synthesized Image
	tar_R = pRefRight->getBuffer();
	memcpy(&tar_R[Height*Width * 0], m_pViewSynthesis1D->GetSynColorRightY(), Height*Width);
	memcpy(&tar_R[Height*Width * 1], m_pViewSynthesis1D->GetSynColorRightU(), Height*Width);
	memcpy(&tar_R[Height*Width * 2], m_pViewSynthesis1D->GetSynColorRightV(), Height*Width);

	// Left Synthesized Depth Image
	memcpy(pRefDepthLeft->getBuffer(), m_pViewSynthesis1D->GetSynDepthLeft(), Height*Width);

	// Right Synthesized Depth Image
	memcpy(pRefDepthRight->getBuffer(), m_pViewSynthesis1D->GetSynDepthRight(), Height*Width);

	m_pBoundaryNoiseRemoval->DepthMatchingWithColor(pRefDepthLeft, pRefLeft, pRefHoleLeft);
	m_pBoundaryNoiseRemoval->DepthMatchingWithColor(pRefDepthRight, pRefRight, pRefHoleRight);
}