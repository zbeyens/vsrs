#include "ViewSynthesis.h"

ViewSynthesis::ViewSynthesis()
{
	m_height = cfg.getSourceHeight();
	m_width = cfg.getSourceWidth();
	m_width2 = m_width * cfg.getPrecision();

	m_iFrameNumber = 0; //Zhejiang
}

ViewSynthesis::~ViewSynthesis()
{
	Tools::safeFree(m_synColorLeft);
	Tools::safeFree(m_synColorRight);
	Tools::safeFree(m_synDepthLeft);
	Tools::safeFree(m_synDepthRight);
}

bool ViewSynthesis::init()
{
	initViews();
	initOutputImages();
	return true;
}

void ViewSynthesis::initViews()
{
	for (size_t i = 0; i < cfg.getNView(); i++)
	{
		m_views.push_back(new View());

		m_views[i]->getImageNotUpsampled()->initSampling(m_height, m_width, IMAGE_CHROMA_FORMAT);
		m_views[i]->getImage()->initSampling(m_height, m_imagesWidth, IMAGE_CHROMA_FORMAT);

		m_views[i]->getDepth()->initSampling(m_height, m_width, DEPTHMAP_CHROMA_FORMAT);
	}
}

void ViewSynthesis::initOutputImages()
{
	// Prepare buffer for boundary noise removal:
	m_synColorLeft = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height * 3)); // in 444
	m_synColorRight = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height * 3)); // in 444
	m_synDepthLeft = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height)); // in 400
	m_synDepthRight = (unsigned char*)malloc(sizeof(unsigned char)*(m_width2*m_height)); // in 400
}

void ViewSynthesis::setImage(int index, unique_ptr<Image<ImageType>>& pcYuv)
{
	if (cfg.getColorSpace() == cfg.COLOR_SPACE_RGB)
		m_views[index]->getImageNotUpsampled()->convertYUVToBGR(pcYuv); // RGB
	else if (cfg.getColorSpace() == cfg.COLOR_SPACE_YUV)
		m_views[index]->getImageNotUpsampled()->convertYUVToYUV444(pcYuv); // YUV
}

void ViewSynthesis::upsample()
{
	AlgoFactory algoFactory;
	Filter<ImageType>* filter = algoFactory.createFilter<ImageType>();

	for (size_t i = 0; i < cfg.getNView(); i++)
	{
		Image<ImageType>* imageNotUpsampled = m_views[i]->getImageNotUpsampled();
		Image<ImageType>* image = m_views[i]->getImage();
		filter->apply(imageNotUpsampled->Y, image->Y, imageNotUpsampled->getWidth(), imageNotUpsampled->getHeight(), 0);
		filter->apply(imageNotUpsampled->U, image->U, imageNotUpsampled->getWidthUV(), imageNotUpsampled->getHeightUV(), 0);
		filter->apply(imageNotUpsampled->V, image->V, imageNotUpsampled->getWidthUV(), imageNotUpsampled->getHeightUV(), 0);
	}
}