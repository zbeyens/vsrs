#include "View.h"

#include "WarpIpelDepth.h"
#include "WarpDepth.h"
#include "WarpViewReverse.h"
#include "WarpIpelViewReverse.h"

View::View()
{
	m_matH_R2V = m_matH_V2R = NULL;
}

View::~View()
{
	xReleaseMemory();

	Tools::safeDelete(m_depthSynthesis);
	Tools::safeDelete(m_viewSynthesisReverse);
}

void View::xReleaseMemory()
{
	Tools::safeReleaseMat(m_matH_R2V);
	Tools::safeReleaseMat(m_matH_V2R);
}

bool View::Init(uint indexSyn)
{
	mIndexSyn = indexSyn;

	xReleaseMemory();

	if (!initIntermImages()) return false;

	init_3Dwarp();

	return true;
}

bool View::initIntermImages()
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	//create images
	m_imgVirtualImage.create(sw, sh, IMAGE_CHANNELS);
	m_imgVirtualDepth.create(sw, sh, DEPTH_CHANNELS);
	m_imgSuccessSynthesis.create(sw, sh, 1);
	m_imgHoles.create(sw, sh, 1);
	m_imgBound.create(sw, sh, 1);
	m_imgMask[0].create(sw, sh, MASK_CHANNELS);
	m_imgMask[1].create(sw, sh, MASK_CHANNELS);

	if (!m_imgVirtualImage.initYUV(m_imgSuccessSynthesis.getImageIpl()->widthStep)) return false;
	if (!m_imgVirtualDepth.initY()) return false;
	if (!m_imgSuccessSynthesis.initY()) return false;

	return true;
}

bool View::init_3Dwarp()
{
	cam.init(mIndexSyn);

	m_dInvZNearMinusInvZFar = 1.0 / cam.getZnear() - 1.0 / cam.getZfar();
	m_dInvZfar = 1.0 / cam.getZfar();

	switch (cfg.getDepthType())
	{
	case 0:
		computeDepthFromCam();
		break;
	case 1:
		computeDepthFromSpace();
		break;
	default:
		return false;
	}

	m_homography.apply(m_matH_R2V, m_matH_V2R, cam.getMatInRef(), cam.getMatExRef(), cam.getMatProjVir());

	if (cfg.getPrecision() == 1)
	{
		m_depthSynthesis = new WarpIpelDepth(this);
		m_viewSynthesisReverse = new WarpIpelViewReverse(this);
	}
	else
	{
		m_depthSynthesis = new WarpDepth(this);
		m_viewSynthesisReverse = new WarpViewReverse(this);
	}

	m_depthSynthesis->init();
	m_viewSynthesisReverse->init();

	return true;
}

void View::computeDepthFromCam()
{
	for (int i = 0; i < MAX_DEPTH; i++)
	{
		double distance = 1.0 / (double(i)*m_dInvZNearMinusInvZFar / (MAX_DEPTH - 1.0) + m_dInvZfar);
		m_dTableD2Z[i] = cvmGet(cam.getMatExRef(), 2, 2) * distance + cam.getArrayTransRef()[2];
	}
}

void View::computeDepthFromSpace()
{
	for (int i = 0; i < MAX_DEPTH; i++)
	{
		double distance = 1.0 / (double(i)*m_dInvZNearMinusInvZFar / (MAX_DEPTH - 1.0) + m_dInvZfar);
		m_dTableD2Z[i] = distance;
	}
}

bool View::xSynthesizeView(ImageType ***src, DepthType **pDepthMap, int th_same_depth)
{
	if (m_depthSynthesis->apply(src, pDepthMap))
	{
		return m_viewSynthesisReverse->apply(src, pDepthMap, th_same_depth);
	}

	return false;
}