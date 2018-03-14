#include "Application.h"

Application::Application()
	: cfg(ConfigSyn::getInstance())
{
	m_synColorLeft = NULL;
	m_synColorRight = NULL;
	m_synDepthLeft = NULL;
	m_synDepthRight = NULL;

	m_iFrameNumber = 0; //Zhejiang
}

Application::~Application()
{
	Tools::safeDelete(m_convertImage);

	Tools::safeFree(m_synColorLeft);
	Tools::safeFree(m_synColorRight);
	Tools::safeFree(m_synDepthLeft);
	Tools::safeFree(m_synDepthRight);
}

bool Application::init()
{
	m_height = cfg.getSourceHeight();
	m_width = cfg.getSourceWidth();
	m_width2 = m_width * cfg.getPrecision();

	if (cfg.getSynthesisMode() == 0)  // General mode
	{
		//m_viewSynthesis3D = ViewSynthesis3D();
		if (!m_viewSynthesis3D.Init()) return false;
		m_convertImage = new ConvertImage3D(&m_viewSynthesis3D, m_height, m_width);
	}
	else  // 1D mode
	{
		//m_viewSynthesis1D = ViewSynthesis1D();

		m_convertImage = new ConvertImage1D(&m_viewSynthesis1D, m_height, m_width2);
	}

	//m_boundaryNoiseRemoval = new BoundaryNoiseRemoval(); // Boundary Noise Removal

	initInputImages();
	initOutputImages();

	return true;
}

void Application::initInputImages()
{
	m_depthMapLeft.init(m_height, m_width, DEPTHMAP_CHROMA_FORMAT);
	m_depthMapRight.init(m_height, m_width, DEPTHMAP_CHROMA_FORMAT);
	
	int imageWidth;
	if (cfg.getSynthesisMode() == 0) // General mode
		imageWidth = m_width2;
	else
		imageWidth = m_width;

	m_imageLeft.init(m_height, m_width2, IMAGE_CHROMA_FORMAT);
	m_imageRight.init(m_height, m_width2, IMAGE_CHROMA_FORMAT);

	//if (cfg.getPrecision() != 1)
	//{
		m_tempImageLeft.init(m_height, m_width, IMAGE_CHROMA_FORMAT);
		m_tempImageRight.init(m_height, m_width, IMAGE_CHROMA_FORMAT);
	//}
	//else
	//{
	//	&m_tempImageLeft = &m_imageLeft;
	//	&m_tempImageRight = &m_imageRight;
	//}
}

void Application::initOutputImages()
{
	// Prepare buffer for boundary noise removal:
	m_synColorLeft = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height * 3)); // in 444
	m_synColorRight = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height * 3)); // in 444
	m_synDepthLeft = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height)); // in 400
	m_synDepthRight = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height)); // in 400
}

bool Application::upsampleImage(int iLeft, ImageData<ImageType> *pcYuv)
{
	if (iLeft)
	{
		if (cfg.getColorSpace())
			m_tempImageLeft.setImage444DataToBGR(pcYuv); // RGB
		else
			m_tempImageLeft.setImage444DataToYUV(pcYuv); // YUV
		m_imageLeft.upsampling(m_tempImageLeft);
	}
	else
	{
		if (cfg.getColorSpace())
			m_tempImageRight.setImage444DataToBGR(pcYuv); // RGB
		else
			m_tempImageRight.setImage444DataToYUV(pcYuv); // YUV
		m_imageRight.upsampling(m_tempImageRight);
	}

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

bool Application::DoViewInterpolation(ImageData<ImageType>& pSynYuvBuffer)
{
	if (cfg.getSynthesisMode() == 0) // General mode
		return xViewSynthesis3D(pSynYuvBuffer);
	else if (cfg.getSynthesisMode() == 1) // 1-D mode
		return xViewSynthesis1D(pSynYuvBuffer);
}

bool Application::xViewSynthesis3D(ImageData<ImageType>& pSynYuvBuffer)
{
	ImageType*** RefLeft = m_imageLeft.getData();
	ImageType*** RefRight = m_imageRight.getData();
	DepthType** RefDepthLeft = m_depthMapLeft.Y;
	DepthType** RefDepthRight = m_depthMapRight.Y;

	if (!m_viewSynthesis3D.apply(RefLeft, RefRight, RefDepthLeft, RefDepthRight, pSynYuvBuffer))
		return false;

	if (cfg.getBoundaryNoiseRemoval()) {
		ImageData<ImageType> synLeftWithHole(m_height, m_width, IMAGE_CHROMA_FORMAT);
		ImageData<ImageType> synRightWithHole(m_height, m_width, IMAGE_CHROMA_FORMAT);
		ImageData<DepthType> synDepthLeftWithHole(m_height, m_width, DEPTHMAP_CHROMA_FORMAT);
		ImageData<DepthType> synDepthRightWithHole(m_height, m_width, DEPTHMAP_CHROMA_FORMAT);
		ImageData<HoleType> synHoleLeft(m_height, m_width, HOLE_CHROMA_FORMAT);
		ImageData<HoleType> synHoleRight(m_height, m_width, HOLE_CHROMA_FORMAT);

		m_convertImage->apply(synLeftWithHole, synRightWithHole, synDepthLeftWithHole, synDepthRightWithHole, synHoleLeft, synHoleRight);
		xBoundaryNoiseRemoval(synLeftWithHole, synRightWithHole, synDepthLeftWithHole, synDepthRightWithHole, synHoleLeft, synHoleRight, pSynYuvBuffer, false/*General Mode*/);
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
bool Application::xViewSynthesis1D(ImageData<ImageType>& pSynYuvBuffer)
{
	ImageType* imageLeft1D = m_imageLeft.getBuffer1D();
	ImageType* imageRight1D = m_imageRight.getBuffer1D();
	DepthType* depthMapLeft1D = m_depthMapLeft.getBuffer1D();
	DepthType* depthMapRight1D = m_depthMapRight.getBuffer1D();

	m_viewSynthesis1D.SetFrameNumber(getFrameNumber()); //Zhejiang
	if (!m_viewSynthesis1D.apply(imageLeft1D, imageRight1D, depthMapLeft1D, depthMapRight1D, pSynYuvBuffer.getBuffer1D()))
		return false;

	if (cfg.getBoundaryNoiseRemoval()) {
		ImageData<ImageType> synColorLeft(m_height, m_width*cfg.getPrecision(), IMAGE_CHROMA_FORMAT);
		ImageData<ImageType> synColorRight(m_height, m_width*cfg.getPrecision(), IMAGE_CHROMA_FORMAT);
		ImageData<DepthType> synDepthLeft(m_height, m_width*cfg.getPrecision(), DEPTHMAP_CHROMA_FORMAT);
		ImageData<DepthType> synDepthRight(m_height, m_width*cfg.getPrecision(), DEPTHMAP_CHROMA_FORMAT);
		ImageData<HoleType> holeLeft(m_height, m_width*cfg.getPrecision(), HOLE1D_CHROMA_FORMAT);
		ImageData<HoleType> holeRight(m_height, m_width*cfg.getPrecision(), HOLE1D_CHROMA_FORMAT);

		m_convertImage->apply(synColorLeft, synColorRight, synDepthLeft, synDepthRight, holeLeft, holeRight);

		m_boundaryNoiseRemoval.DepthMatchingWithColor(synDepthLeft, synColorLeft, holeLeft);
		m_boundaryNoiseRemoval.DepthMatchingWithColor(synDepthRight, synColorRight, holeRight);
		xBoundaryNoiseRemoval(synColorLeft, synColorRight, synDepthLeft, synDepthRight, holeLeft, holeRight, pSynYuvBuffer, true/*1D Mode*/);
	}

	return true;
}

bool Application::xBoundaryNoiseRemoval(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight, ImageData<HoleType>& pRefHoleLeft, ImageData<HoleType>& pRefHoleRight, ImageData<ImageType>& pSynYuvBuffer, bool SynthesisMode)
{
	switch (SynthesisMode)
	{
	case false:  // General Mode
		m_boundaryNoiseRemoval.SetLeftBaseLineDist(m_viewSynthesis3D.GetViewLeft()->getCam().getBaselineDistance());
		m_boundaryNoiseRemoval.SetRightBaseLineDist(m_viewSynthesis3D.GetViewRight()->getCam().getBaselineDistance());
		m_boundaryNoiseRemoval.SetLeftH_V2R(m_viewSynthesis3D.GetViewLeft()->getMatH_V2R());
		m_boundaryNoiseRemoval.SetRightH_V2R(m_viewSynthesis3D.GetViewRight()->getMatH_V2R());
		break;
	case true:   // 1D Mode
		m_boundaryNoiseRemoval.SetduPrincipal(m_viewSynthesis1D.GetduPrincipal());
		m_boundaryNoiseRemoval.SetLTranslationLeft(m_viewSynthesis1D.GetLTranslation());
		m_boundaryNoiseRemoval.SetZfar(m_viewSynthesis1D.GetZfar());
		m_boundaryNoiseRemoval.SetZnear(m_viewSynthesis1D.GetZnear());
		break;
	}

	if (!m_boundaryNoiseRemoval.DoBoundaryNoiseRemoval(pRefLeft, pRefRight, pRefDepthLeft, pRefDepthRight, pRefHoleLeft, pRefHoleRight, pSynYuvBuffer, SynthesisMode))
		return false;

	return true;
}