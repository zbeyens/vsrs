//#include "Application.h"
//
//Application::Application()
//	: cfg(ConfigSyn::getInstance())
//{
//	
//}
//
//Application::~Application()
//{
//	Tools::safeFree(m_synColorLeft);
//	Tools::safeFree(m_synColorRight);
//	Tools::safeFree(m_synDepthLeft);
//	Tools::safeFree(m_synDepthRight);
//}
//
//bool Application::init()
//{
//	m_height = cfg.getSourceHeight();
//	m_width = cfg.getSourceWidth();
//	m_width2 = m_width * cfg.getPrecision();
//
//	Image<ImageType> img1;
//	Image<ImageType> img2;
//	Image<ImageType> img3;
//	Image<ImageType> img4;
//
//	m_imagesNotUpsampled.push_back(Image<ImageType>());
//	m_imagesNotUpsampled.push_back(Image<ImageType>());
//	m_images.push_back(Image<ImageType>());
//	m_images.push_back(Image<ImageType>());
//
//	if (cfg.getSynthesisMode() == cfg.MODE_3D)  // General mode
//	{
//		//m_viewSynthesis3D = ViewSynthesis3D();
//		if (!m_viewSynthesis3D.Init()) return false;
//	}
//	else if (cfg.getSynthesisMode() == cfg.MODE_1D)  // 1D mode
//	{
//		//m_viewSynthesis1D = ViewSynthesis1D();
//	}
//
//	//m_boundaryNoiseRemoval = new BoundaryNoiseRemoval(); // Boundary Noise Removal
//
//	initInputImages();
//	initOutputImages();
//
//	return true;
//}
//
//void Application::initInputImages()
//{
//	m_depthLeft.initSampling(m_height, m_width, DEPTHMAP_CHROMA_FORMAT);
//	m_depthRight.initSampling(m_height, m_width, DEPTHMAP_CHROMA_FORMAT);
//	
//	int imageWidth;
//	if (cfg.getSynthesisMode() == cfg.MODE_3D) // General mode
//		imageWidth = m_width2;
//	else if (cfg.getSynthesisMode() == cfg.MODE_1D)
//		imageWidth = m_width;
//
//	m_images[0].initSampling(m_height, m_width2, IMAGE_CHROMA_FORMAT);
//	m_images[1].initSampling(m_height, m_width2, IMAGE_CHROMA_FORMAT);
//
//	//if (cfg.getPrecision() != 1)
//	//{
//	m_imagesNotUpsampled[0].initSampling(m_height, m_width, IMAGE_CHROMA_FORMAT);
//	m_imagesNotUpsampled[1].initSampling(m_height, m_width, IMAGE_CHROMA_FORMAT);
//	//}
//	//else
//	//{
//	//	&m_tempImageLeft = &m_imageLeft;
//	//	&m_tempImageRight = &m_imageRight;
//	//}
//}
//
//void Application::initOutputImages()
//{
//	// Prepare buffer for boundary noise removal:
//	m_synColorLeft = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height * 3)); // in 444
//	m_synColorRight = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height * 3)); // in 444
//	m_synDepthLeft = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height)); // in 400
//	m_synDepthRight = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height)); // in 400
//}
//
//bool Application::upsampleImage(int index, unique_ptr<Image<ImageType>>& pcYuv)
//{
//	AlgoFactory algoFactory;
//	Filter<ImageType>* filter = algoFactory.createFilter<ImageType>();
//
//	if (cfg.getColorSpace() == cfg.COLOR_SPACE_RGB)
//		m_imagesNotUpsampled[index].convertYUV444ToBGR(pcYuv); // RGB
//	else if (cfg.getColorSpace() == cfg.COLOR_SPACE_YUV)
//		m_imagesNotUpsampled[index].convertYUV444ToYUV(pcYuv); // YUV
//
//	filter->apply(m_imagesNotUpsampled[index].Y, m_images[index].Y, m_imagesNotUpsampled[index].getWidth(), m_imagesNotUpsampled[index].getHeight(), 0);
//	filter->apply(m_imagesNotUpsampled[index].U, m_images[index].U, m_imagesNotUpsampled[index].getWidthUV(), m_imagesNotUpsampled[index].getHeightUV(), 0);
//	filter->apply(m_imagesNotUpsampled[index].V, m_images[index].V, m_imagesNotUpsampled[index].getWidthUV(), m_imagesNotUpsampled[index].getHeightUV(), 0);
//
//	return true;
//}
//
///*
// * \brief
// * The main interface function to perform view interpolation
// *   to be called from the application
// *
// * \param pSynYuvBuffer
// *   Store the synthesized picture into the buffer
// *
// * \return
// *    true: if succeed;
// *    false: if fails.
// */
//
//bool Application::DoViewInterpolation(unique_ptr<Image<ImageType>>& pSynYuvBuffer)
//{
//	if (cfg.getSynthesisMode() == cfg.MODE_3D) // General mode
//		return xViewSynthesis3D(pSynYuvBuffer);
//	else if (cfg.getSynthesisMode() == cfg.MODE_1D) // 1-D mode
//		return xViewSynthesis1D(pSynYuvBuffer);
//}
//
//bool Application::xViewSynthesis3D(unique_ptr<Image<ImageType>>& pSynYuvBuffer)
//{
//	if (!m_viewSynthesis3D.apply(m_images[0], m_images[1], m_depthLeft, m_depthRight, pSynYuvBuffer))
//		return false;
//
//	if (cfg.getBoundaryNoiseRemoval() == cfg.BNR_ENABLED) {
//		xBoundaryNoiseRemoval(m_viewSynthesis3D.getSynImageLeftWithHole(), m_viewSynthesis3D.getSynImageRightWithHole(), m_viewSynthesis3D.getSynDepthLeftWithHole(), m_viewSynthesis3D.getSynDepthRightWithHole(), m_viewSynthesis3D.getSynHolesLeft(), m_viewSynthesis3D.getSynHolesRight(), pSynYuvBuffer, false/*General Mode*/);
//	}
//
//	return true;
//}
//
///*
// * To perform 1-D view interpolation
// *
// * \Output:
// *    All the color images below are in 444, depth images are 400
// * \param pSynYuvBuffer
// *    To store the synthesis result.
// * \member m_pSynColorLeft
// *    To store the synthesis color component from left ref w/o hole filling
// * \member m_pSynColorRight
// *    To store the synthesis color component from right ref w/o hole filling
// * \member m_pSynDepthLeft
// *    To store the synthesis depth component from left ref w/o hole filling
// * \member m_pSynDepthRight
// *    To store the synthesis depth component from right ref w/o hole filling
// *
// * Return:
// *    true:  if succeed;
// *    false: if fails.
// */
//bool Application::xViewSynthesis1D(unique_ptr<Image<ImageType>>& pSynYuvBuffer)
//{
//	m_viewSynthesis1D.SetFrameNumber(getFrameNumber()); //Zhejiang
//	if (!m_viewSynthesis1D.apply(m_images[0], m_images[1], m_depthLeft, m_depthRight, pSynYuvBuffer))
//		return false;
//
//	if (cfg.getBoundaryNoiseRemoval() == cfg.BNR_ENABLED) {
//		xBoundaryNoiseRemoval(m_viewSynthesis1D.GetSynLeftWithHole(), m_viewSynthesis1D.GetSynRightWithHole(), m_viewSynthesis1D.GetSynDepthLeftWithHole(), m_viewSynthesis1D.GetSynDepthRightWithHole(), m_viewSynthesis1D.GetSynHoleLeft(), m_viewSynthesis1D.GetSynHoleRight(), pSynYuvBuffer, true/*1D Mode*/);
//	}
//
//	return true;
//}
//
//bool Application::xBoundaryNoiseRemoval(Image<ImageType>* pRefLeft, Image<ImageType>* pRefRight, Image<DepthType>* pRefDepthLeft, Image<DepthType>* pRefDepthRight, Image<HoleType>* pRefHoleLeft, Image<HoleType>* pRefHoleRight, unique_ptr<Image<ImageType>>& pSynYuvBuffer, bool SynthesisMode)
//{
//	switch (SynthesisMode)
//	{
//	case false:  // General Mode
//		m_boundaryNoiseRemoval.SetLeftBaseLineDist(m_viewSynthesis3D.getViewLeft()->getCam().getBaselineDistance());
//		m_boundaryNoiseRemoval.SetRightBaseLineDist(m_viewSynthesis3D.getViewRight()->getCam().getBaselineDistance());
//		m_boundaryNoiseRemoval.SetLeftH_V2R(m_viewSynthesis3D.getViewLeft()->getMatH_V2R());
//		m_boundaryNoiseRemoval.SetRightH_V2R(m_viewSynthesis3D.getViewRight()->getMatH_V2R());
//		break;
//	case true:   // 1D Mode
//		m_boundaryNoiseRemoval.SetduPrincipal(m_viewSynthesis1D.GetduPrincipal());
//		m_boundaryNoiseRemoval.SetLTranslationLeft(m_viewSynthesis1D.GetLTranslation());
//		m_boundaryNoiseRemoval.SetZfar(m_viewSynthesis1D.GetZfar());
//		m_boundaryNoiseRemoval.SetZnear(m_viewSynthesis1D.GetZnear());
//		break;
//	}
//
//	if (!m_boundaryNoiseRemoval.apply(pRefLeft, pRefRight, pRefDepthLeft, pRefDepthRight, pRefHoleLeft, pRefHoleRight, pSynYuvBuffer, SynthesisMode))
//		return false;
//
//	return true;
//}