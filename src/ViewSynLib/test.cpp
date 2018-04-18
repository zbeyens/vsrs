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
	Tools::safeDelete(m_viewLeft);
	Tools::safeDelete(m_viewRight);

	Tools::safeDelete(m_synImageLeftWithHole);
	Tools::safeDelete(m_synImageRightWithHole);
	Tools::safeDelete(m_synDepthLeftWithHole);
	Tools::safeDelete(m_synDepthRightWithHole);
	Tools::safeDelete(m_synHolesLeft);
	Tools::safeDelete(m_synHolesRight);
}

bool ViewSynthesis3D::Init()
{
	m_viewLeft = new View();
	m_viewRight = new View();

	if (!m_viewLeft->Init(0))  return false;
	if (!m_viewRight->Init(1))  return false;

	computeWeightLR();

	initResultImages();

	return true;
}

void ViewSynthesis3D::computeWeightLR()
{
	m_weightLeft = m_viewRight->getCam().getBaselineDistance();
	m_weightRight = m_viewLeft->getCam().getBaselineDistance();
	m_totalBaseline = m_weightLeft + m_weightRight;
	m_weightLeft /= m_totalBaseline;
	m_weightRight /= m_totalBaseline;
}

void ViewSynthesis3D::initResultImages()
{
	int width = cfg.getSourceWidth();
	int height = cfg.getSourceHeight();

	m_synImageLeftWithHole = new Image<ImageType>(height, width, IMAGE_CHROMA_FORMAT);
	m_synImageRightWithHole = new Image<ImageType>(height, width, IMAGE_CHROMA_FORMAT);
	m_synDepthLeftWithHole = new Image<DepthType>(height, width, DEPTHMAP_CHROMA_FORMAT);
	m_synDepthRightWithHole = new Image<DepthType>(height, width, DEPTHMAP_CHROMA_FORMAT);
	m_synHolesLeft = new Image<HoleType>(height, width, HOLE_CHROMA_FORMAT);
	m_synHolesRight = new Image<HoleType>(height, width, HOLE_CHROMA_FORMAT);
	m_synImageLeftWithHole->initMat(width, height, 3);
	m_synImageRightWithHole->initMat(width, height, 3);
	m_synDepthLeftWithHole->initMat(width, height, 1);
	m_synDepthRightWithHole->initMat(width, height, 1);
	m_synHolesLeft->initMat(width, height, 1);
	m_synHolesRight->initMat(width, height, 1);

	m_blendedImage->initMat(width, height, IMAGE_CHANNELS);
	m_blendedDepth->initMat(width, height, BLENDED_DEPTH_CHANNELS);
	m_synImage->initMat(width, height, IMAGE_CHANNELS);
	m_holesMask->initMat(width, height, MASK_CHANNELS);
	m_fillableHolesMasks[0].initMat(width, height, MASK_CHANNELS);
	m_fillableHolesMasks[1].initMat(width, height, MASK_CHANNELS);
}

void ViewSynthesis3D::initMasks()
{
	if (cfg.getIvsrsInpaint() == 1)
	{
		cvAnd(m_viewLeft->getSynHolesMat(), m_viewRight->getSynFillsMat(), m_fillableHolesMasks[0].getMat()); // NICT use same hole mask
		cvAnd(m_viewRight->getSynHolesMat(), m_viewLeft->getSynFillsMat(), m_fillableHolesMasks[1].getMat());
	}
	else
	{
		cvAnd(m_viewLeft->getUnstablePixelsMat(), m_viewRight->getSynFillsMat(), m_fillableHolesMasks[0].getMat()); // Left dilated Mask[0] * Right Success -> Left Mask[3] // dilated hole fillable by Right
		cvAnd(m_viewRight->getUnstablePixelsMat(), m_viewLeft->getSynFillsMat(), m_fillableHolesMasks[1].getMat()); // Right dilated Mask[0] * Left Success -> Mask[4] // dilated hole fillable by Left
	}

	// pixels which couldn't be synthesized from both left and right -> inpainting
	cvAnd(m_viewLeft->getSynHolesMat(), m_viewRight->getSynHolesMat(), m_holesMask->getMat()); // Left Hole * Right Hole -> Mask[2] // common hole,

}

bool ViewSynthesis3D::isPixelNearer(DepthType left, DepthType right)
{
	return left > right;
}

void ViewSynthesis3D::blendNearerPixel(Image<ImageType> blendedImage, Image<DepthType> blendedDepth, Image<ImageType>* image, Image<DepthType>* depth, int ptv)
{
	for (size_t j = 0; j < 3; j++)
	{
		blendedImage.setMatData(ptv * 3 + j, image->getMatData(ptv * 3 + j));
	}

	if (cfg.getIvsrsInpaint() == 1)
	{
		blendedDepth.setMatData(ptv, depth->getMatData(ptv));
	}
}

bool ViewSynthesis3D::apply(Image<ImageType>& RefLeft, Image<ImageType>& RefRight, Image<DepthType>& RefDepthLeft, Image<DepthType>& RefDepthRight, Image<ImageType>& pSynYuvBuffer)
{
	ImageType*** pRefLeft = RefLeft.getData();
	ImageType*** pRefRight = RefRight.getData();
	DepthType** pRefDepthLeft = RefDepthLeft.Y;
	DepthType** pRefDepthRight = RefDepthRight.Y;

	if (!m_viewLeft->xSynthesizeView(pRefLeft, pRefDepthLeft))  return false;
	if (!m_viewRight->xSynthesizeView(pRefRight, pRefDepthRight)) return false;

	// copy warping results for BNR
	m_synImageLeftWithHole->convertMatToBuffer1D(m_viewLeft->getSynImage(), 3);
	m_synImageRightWithHole->convertMatToBuffer1D(m_viewRight->getSynImage(), 3);
	m_synDepthLeftWithHole->convertMatToBuffer1D(m_viewLeft->getSynDepth(), 1);
	m_synDepthRightWithHole->convertMatToBuffer1D(m_viewRight->getSynDepth(), 1);
	m_synHolesLeft->convertMatToBuffer1D(m_viewLeft->getSynHoles(), 1);
	m_synHolesRight->convertMatToBuffer1D(m_viewRight->getSynHoles(), 1);

	initMasks();

	if (cfg.getViewBlending() == 1)
	{
		BlendingCloser blendingCloser;
		blendingCloser.apply(m_viewLeft->getSynImage(), m_viewRight->getSynImage(), m_viewLeft->getSynFills(), m_viewRight->getSynFills(), m_fillableHolesMasks, m_weightLeft, m_weightRight);
	}
	else {
		BlendingHoles blendingHoles;
		blendingHoles.apply(m_viewLeft->getSynImage(), m_viewRight->getSynImage(), m_viewLeft->getSynDepth(), m_viewRight->getSynDepth(), m_fillableHolesMasks);
	}

	cout << "hihi" << endl;
	Blending blending;
	//blending.apply(m_blendedImage, m_blendedDepth, m_viewLeft->getSynImage(), m_viewRight->getSynImage(), m_viewLeft->getSynDepth(), m_viewRight->getSynDepth(), m_holesMask, m_weightLeft, m_weightRight, m_totalBaseline);
	
	for (int h = 0; h < cfg.getSourceHeight(); h++)
	{
		for (int w = 0; w < cfg.getSourceWidth(); w++)
		{
			int ptv = w + h * cfg.getSourceWidth();

			for (size_t j = 0; j < 3; j++)
			{
				m_blendedImage->setMatData(ptv * 3 + j, 0);
			}

			if (m_holesMask->getMatData(ptv) != 0) continue;

			if ((abs((m_viewLeft->getSynDepth()->getMatData(ptv)) - (m_viewRight->getSynDepth()->getMatData(ptv))) < cfg.getDepthBlendDiff())) // left and right are close to each other (NICT)
			{
				for (size_t j = 0; j < 3; j++)
				{
					m_blendedImage->setMatData(ptv * 3 + j, ImageTools::CLIP3((m_viewLeft->getSynImage()->getMatData(ptv * 3 + j) * m_weightLeft + m_viewRight->getSynImage()->getMatData(ptv * 3 + j) * m_weightRight) / m_totalBaseline, 0, MAX_LUMA - 1));
				}

				if (cfg.getIvsrsInpaint() == 1)
				{
					m_blendedDepth->setMatData(ptv, ImageTools::CLIP3((m_viewLeft->getSynDepth()->getMatData(ptv) * m_weightLeft + m_viewRight->getSynDepth()->getMatData(ptv) * m_weightRight) / m_totalBaseline, 0, MAX_DEPTH - 1));
				}
			}
			else if (isPixelNearer(m_viewLeft->getSynDepth()->getMatData(ptv), m_viewRight->getSynDepth()->getMatData(ptv)))
			{
				//blendNearerPixel(m_blendedImage, m_blendedDepth, m_viewLeft->getSynImage(), m_viewLeft->getSynDepth(), ptv);
			}
			else
			{
				//blendNearerPixel(m_blendedImage, m_blendedDepth, m_viewRight->getSynImage(), m_viewRight->getSynDepth(), ptv);
			}
		}
	}

	cout << "huhu" << endl;
	
	Inpaint inpaint;
	if (cfg.getIvsrsInpaint() == 1)
	{
		inpaint.apply(m_synImage, m_blendedImage, m_blendedDepth, m_holesMask, m_fillableHolesMasks);
	}
	else
	{
		inpaint.applyDefault(m_synImage, m_blendedImage, m_holesMask);
	}

	if (cfg.getColorSpace()) {
		pSynYuvBuffer.convertMatBGRToYUV(m_synImage->getMat());
	}
	else
		pSynYuvBuffer.convertMatYUVToYUV(m_synImage->getMat());

	return true;
}