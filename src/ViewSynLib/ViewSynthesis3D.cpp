#include "ViewSynthesis3D.h"

ViewSynthesis3D::ViewSynthesis3D()
	: cfg(ConfigSyn::getInstance())
{
}

ViewSynthesis3D::~ViewSynthesis3D()
{
	xReleaseMemory();
}

void ViewSynthesis3D::xReleaseMemory()
{
	Tools::safeDelete(m_blendedImage);
	Tools::safeDelete(m_synDepth);
	Tools::safeDelete(m_synImage);
	Tools::safeDelete(m_holesMask);
}

bool ViewSynthesis3D::init()
{
	ViewSynthesis::init();

	computeWeights();

	initResultImages();

	return true;
}

void ViewSynthesis3D::computeWeights()
{
	double totalBaseline = 0;
	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		totalBaseline += 1 / m_views[i]->getCam().getBaseline();
	}

	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		m_views[i]->setWeight(1 / m_views[i]->getCam().getBaseline() / totalBaseline);
	}
}

void ViewSynthesis3D::initResultImages()
{
	int width = cfg.getSourceWidth();
	int height = cfg.getSourceHeight();

	m_blendedImage = new Image<ImageType>();
	m_synDepth = new Image<DepthType>(width, height, DEPTHMAP_CHROMA_FORMAT);
	m_synImage = new Image<ImageType>(width, height, IMAGE_CHROMA_FORMAT);
	m_holesMask = new Image<ImageType>();
	m_blendedImage->initMat(width, height, IMAGE_CHANNELS);
	m_synDepth->initMat(width, height, BLENDED_DEPTH_CHANNELS);
	m_synImage->initMat(width, height, IMAGE_CHANNELS);
	m_holesMask->initMat(width, height, MASK_CHANNELS);
}

bool ViewSynthesis3D::apply(unique_ptr<Image<ImageType>>& outImg)
{
	AlgoFactory algoFactory;

	upsampleViews();

	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		Warp* depthSynthesis = algoFactory.createWarpDepth();
		Warp* viewSynthesisReverse = algoFactory.createWarpViewReverse();

		depthSynthesis->apply(m_views[i]);
		viewSynthesisReverse->apply(m_views[i]);
	}

	BlendingHoles* blendingHoles = algoFactory.createBlendingHoles();
	blendingHoles->apply(m_views, m_holesMask);

	Blending* blending = algoFactory.createBlending();
	blending->apply(m_views, m_blendedImage, m_synDepth, m_holesMask);

	Inpaint* inpaint = algoFactory.createInpaint();
	inpaint->apply(m_synImage, m_blendedImage, m_synDepth, m_holesMask, m_views);

	if (cfg.getColorSpace() == cfg.COLOR_SPACE_RGB)
		outImg->convertMatBGRToYUV(m_synImage->getMat());
	else if (cfg.getColorSpace() == cfg.COLOR_SPACE_YUV)
		outImg->convertMatYUVToYUV(m_synImage->getMat());

	//m_synDepth->convertMatYUVToYUV(m_synDepth->getMat());

	return true;
}