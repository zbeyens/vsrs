#include "ViewSynthesis3D.h"



ViewSynthesis3D::ViewSynthesis3D()
	: cfg(ConfigSyn::getInstance())
{
	m_imagesWidth = m_width2;
}

ViewSynthesis3D::~ViewSynthesis3D()
{
	xReleaseMemory();
}

void ViewSynthesis3D::xReleaseMemory()
{
	for (size_t i = 0; i < cfg.getNView(); i++)
	{
		Tools::safeDelete(m_views[i]);
	}

	Tools::safeDelete(m_blendedImage);
	Tools::safeDelete(m_blendedDepth);
	Tools::safeDelete(m_synImage);
	Tools::safeDelete(m_holesMask);
}

bool ViewSynthesis3D::init()
{
	ViewSynthesis::init();

	for (size_t i = 0; i < cfg.getNView(); i++)
	{
		if (!m_views[i]->init(i))  return false;
	}

	computeWeights();

	initResultImages();

	return true;
}

void ViewSynthesis3D::computeWeights()
{
	m_totalBaseline = 0;
	for (size_t i = 0; i < cfg.getNView(); i++)
	{
		m_totalBaseline += m_views[i]->getCam().getBaseline();
	}
	for (size_t i = 0; i < cfg.getNView(); i++)
	{
		//m_views[i]->setWeight(m_views[i]->getCam().getBaseline() / m_totalBaseline);
	}
	m_views[0]->setWeight(m_views[1]->getCam().getBaseline() / m_totalBaseline);
	m_views[1]->setWeight(m_views[0]->getCam().getBaseline() / m_totalBaseline);
}

void ViewSynthesis3D::initResultImages()
{
	int width = cfg.getSourceWidth();
	int height = cfg.getSourceHeight();

	m_blendedImage = new Image<ImageType>();
	m_blendedDepth = new Image<DepthType>();
	m_synImage = new Image<ImageType>();
	m_holesMask = new Image<ImageType>();
	m_blendedImage->initMat(width, height, IMAGE_CHANNELS);
	m_blendedDepth->initMat(width, height, BLENDED_DEPTH_CHANNELS);
	m_synImage->initMat(width, height, IMAGE_CHANNELS);
	m_holesMask->initMat(width, height, MASK_CHANNELS);
}

void ViewSynthesis3D::initMasks()
{
	if (cfg.getIvsrsInpaint() == 1)
	{
		cvAnd(m_views[0]->getSynHolesMat(), m_views[1]->getSynFillsMat(), m_views[0]->getFillableHoles()->getMat()); // NICT use same hole mask
		cvAnd(m_views[1]->getSynHolesMat(), m_views[0]->getSynFillsMat(), m_views[1]->getFillableHoles()->getMat());
	}
	else
	{
		cvAnd(m_views[0]->getUnstablePixelsMat(), m_views[1]->getSynFillsMat(), m_views[0]->getFillableHoles()->getMat()); // Left dilated Mask[0] * Right Success -> Left Mask[3] // dilated hole fillable by Right
		cvAnd(m_views[1]->getUnstablePixelsMat(), m_views[0]->getSynFillsMat(), m_views[1]->getFillableHoles()->getMat()); // Right dilated Mask[0] * Left Success -> Mask[4] // dilated hole fillable by Left
	}

	// pixels which couldn't be synthesized from both left and right -> inpainting
	cvAnd(m_views[0]->getSynHolesMat(), m_views[1]->getSynHolesMat(), m_holesMask->getMat()); // Left Hole * Right Hole -> Mask[2] // common hole,
}

bool ViewSynthesis3D::apply(unique_ptr<Image<ImageType>>& pSynYuvBuffer)
{
	upsample();

	for (size_t i = 0; i < cfg.getNView(); i++)
	{
		m_views[i]->synthesizeView();
	}

	initMasks();

	BlendingHoles* blendingHoles = AlgoFactory::createBlendingHoles();
	blendingHoles->apply(m_views);

	Blending* blending = new Blending();
	blending->apply(m_views, m_blendedImage, m_blendedDepth, m_holesMask, m_totalBaseline);

	//Image<ImageType>* pRefLeft = m_views[0]->getSynImage();
	//Image<ImageType>* pRefRight = m_views[1]->getSynImage();
	//Image<DepthType>* pRefDepthLeft = m_views[0]->getSynDepth();
	//Image<DepthType>* pRefDepthRight = m_views[1]->getSynDepth();
	//Image<HoleType>* pRefFillsLeft = m_views[0]->getSynFills();
	//Image<HoleType>* pRefFillsRight = m_views[1]->getSynFills();

	Inpaint* inpaint = AlgoFactory::createInpaint();
	inpaint->apply(m_synImage, m_blendedImage, m_blendedDepth, m_holesMask, m_views);

	if (cfg.getColorSpace() == cfg.COLOR_SPACE_RGB)
		pSynYuvBuffer->convertMatBGRToYUV(m_blendedImage->getMat());
	else if (cfg.getColorSpace() == cfg.COLOR_SPACE_YUV)
		pSynYuvBuffer->convertMatYUVToYUV(m_blendedImage->getMat());

	cout << "BNR";

	//BoundaryNoiseRemoval* boundaryNoiseRemoval = new BoundaryNoiseRemoval3D();
	//boundaryNoiseRemoval->SetLeftBaseLineDist(getViewLeft()->getCam().getBaseline());
	//boundaryNoiseRemoval->SetRightBaseLineDist(getViewRight()->getCam().getBaseline());
	//boundaryNoiseRemoval->SetLeftH_V2R(getViewLeft()->getMatH_V2R());
	//boundaryNoiseRemoval->SetRightH_V2R(getViewRight()->getMatH_V2R());
	//if (!boundaryNoiseRemoval->apply(m_views[0]->getSynImageWithHole(), m_views[1]->getSynImageWithHole(), m_views[0]->getSynDepthWithHole(), m_views[1]->getSynDepthWithHole(), m_views[0]->getSynHoles(), m_views[1]->getSynHoles(), pSynYuvBuffer, false))
	//	return false;

	return true;
}