#include "ViewSynthesis3D.h"

ViewSynthesis3D::ViewSynthesis3D()
	: cfg(ConfigSyn::getInstance())
{
}

ViewSynthesis3D::~ViewSynthesis3D()
{
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
		totalBaseline += 1 / m_views[i]->getCam()->getBaseline();
	}

	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		m_views[i]->setWeight(1 / m_views[i]->getCam()->getBaseline() / totalBaseline);
	}
}

void ViewSynthesis3D::initResultImages()
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	m_blendedImage = shared_ptr<Image<ImageType>>(new Image<ImageType>());
	m_synDepth = shared_ptr<Image<DepthType>>(new Image<DepthType>(sh, sw, DEPTHMAP_CHROMA_FORMAT));
	m_synImage = shared_ptr<Image<ImageType>>(new Image<ImageType>(sh, sw, IMAGE_CHROMA_FORMAT));
	m_holesMask = shared_ptr<Image<ImageType>>(new Image<ImageType>());
	m_blendedImage->initMat(sw, sh, IMAGE_CHANNELS);
	m_synDepth->initMat(sw, sh, BLENDED_DEPTH_CHANNELS);
	m_synImage->initMat(sw, sh, IMAGE_CHANNELS);
	m_holesMask->initMat(sw, sh, MASK_CHANNELS);
}

bool ViewSynthesis3D::apply(shared_ptr<Image<ImageType>> outImg)
{
	AlgoFactory algoFactory;

	upsampleViews();

	unique_ptr<Warp> depthSynthesis(algoFactory.createWarpDepth());
	unique_ptr<Warp> viewSynthesisReverse(algoFactory.createWarpViewReverse());
	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		depthSynthesis->apply(m_views[i]);
		viewSynthesisReverse->apply(m_views[i]);
	}

	unique_ptr<BlendingHoles> blendingHoles(algoFactory.createBlendingHoles());
	blendingHoles->apply(m_views, m_holesMask);

	unique_ptr<Blending> blending(algoFactory.createBlending());
	blending->apply(m_views, m_blendedImage, m_synDepth, m_holesMask);

	unique_ptr<Inpaint> inpaint(algoFactory.createInpaint());
	inpaint->apply(m_synImage, m_blendedImage, m_synDepth, m_holesMask, m_views);

	if (cfg.getColorSpace() == cfg.COLOR_SPACE_RGB)
		outImg->convertMatBGRToYUV(m_synImage->getMat());
	else if (cfg.getColorSpace() == cfg.COLOR_SPACE_YUV)
		outImg->convertMatYUVToYUV(m_synImage->getMat());

	//m_synDepth->convertMatYUVToYUV(m_synDepth->getMat());

	return true;
}