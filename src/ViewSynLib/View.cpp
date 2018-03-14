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
	m_indexSyn = indexSyn;

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
	cam.init(m_indexSyn);

	m_invZNearMinusInvZFar = 1.0 / cam.getZnear() - 1.0 / cam.getZfar();
	m_invZfar = 1.0 / cam.getZfar();

	computeDepth();

	m_homography.apply(m_matH_R2V, m_matH_V2R, cam.getMatInRef(), cam.getMatExRef(), cam.getMatProjVir());

	if (cfg.getPrecision() == 1)
	{
		//m_depthSynthesis = new WarpIpelDepth(this); //does not work for 16 BitDepth
		m_depthSynthesis = new WarpDepth(this);
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

bool View::computeDepth()
{
	for (int i = 0; i < MAX_DEPTH; i++)
	{
		double distance = 1.0 / (double(i)*m_invZNearMinusInvZFar / (MAX_DEPTH - 1.0) + m_invZfar);
		switch (cfg.getDepthType())
		{
		case 0:
			computeDepthFromCam(i, distance);
			break;
		case 1:
			computeDepthFromSpace(i, distance);
			break;
		default:
			return false;
		}
	}
}

void View::computeDepthFromCam(int i, double distance)
{
	m_tableD2Z[i] = cvmGet(cam.getMatExRef(), 2, 2) * distance + cam.getArrayTransRef()[2];
}

void View::computeDepthFromSpace(int i, double distance)
{
	m_tableD2Z[i] = distance;
}

bool View::xSynthesizeView(ImageType ***src, DepthType **pDepthMap)
{
	if (m_depthSynthesis->apply(pDepthMap))
	{
		return m_viewSynthesisReverse->apply(src);
	}

	return false;
}