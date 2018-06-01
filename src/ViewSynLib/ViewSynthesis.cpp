#include "ViewSynthesis.h"

ViewSynthesis::ViewSynthesis()
{
	m_frameNumber = 0; //Zhejiang
}

ViewSynthesis::~ViewSynthesis()
{
}

bool ViewSynthesis::init()
{
	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		m_views.push_back(shared_ptr<View>(new View()));

		if (!m_views[i]->init(i)) return false;
	}
	return true;
}

void ViewSynthesis::setInputImage(int index, shared_ptr<Image<ImageType>> src)
{
	if (cfg.getColorSpace() == cfg.COLOR_SPACE_RGB)
		m_views[index]->getImageNotUpsampled()->convertYUVToBGR(src); // RGB
	else if (cfg.getColorSpace() == cfg.COLOR_SPACE_YUV)
		m_views[index]->getImageNotUpsampled()->convertYUVToYUV444(src); // YUV
}

void ViewSynthesis::upsampleViews()
{
	AlgoFactory algoFactory;
	unique_ptr<Filter<ImageType>> horizontalFilter(algoFactory.createFilter<ImageType>(true));
	unique_ptr<Filter<ImageType>> verticalFilter(algoFactory.createFilter<ImageType>(false));

	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		shared_ptr<Image<ImageType>> imageNotUpsampled = m_views[i]->getImageNotUpsampled();
		shared_ptr<Image<ImageType>> imageHorUpsampled = m_views[i]->getImageHorUpsampled();
		shared_ptr<Image<ImageType>> image = m_views[i]->getImage();
		horizontalFilter->apply(imageNotUpsampled->getY(), image->getY(), imageNotUpsampled->getWidth(), imageNotUpsampled->getHeight(), 0);
		horizontalFilter->apply(imageNotUpsampled->getU(), image->getU(), imageNotUpsampled->getWidthUV(), imageNotUpsampled->getHeightUV(), 0);
		horizontalFilter->apply(imageNotUpsampled->getV(), image->getV(), imageNotUpsampled->getWidthUV(), imageNotUpsampled->getHeightUV(), 0);

		//verticalFilter->apply(imageHorUpsampled->getY(), image->getY(), imageHorUpsampled->getWidth(), imageHorUpsampled->getHeight(), 0);
		//verticalFilter->apply(imageHorUpsampled->getU(), image->getU(), imageHorUpsampled->getWidthUV(), imageHorUpsampled->getHeightUV(), 0);
		//verticalFilter->apply(imageHorUpsampled->getV(), image->getV(), imageHorUpsampled->getWidthUV(), imageHorUpsampled->getHeightUV(), 0);
		//image = imageHorUpsampled;
	}
}