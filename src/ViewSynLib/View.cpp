#include "View.h"

#include "WarpIpelDepth.h"
#include "WarpDepth.h"
#include "WarpViewReverse.h"
#include "WarpIpelViewReverse.h"

View::View()
{
	m_matH_R2V = m_matH_V2R = NULL;

	m_imageNotUpsampled = new Image<ImageType>();
	m_image = new Image<ImageType>();
	m_depth = new Image<DepthType>();

	int width = cfg.getSourceWidth();
	int height = cfg.getSourceHeight();

	m_synImageWithHole = new Image<ImageType>(height, width, IMAGE_CHROMA_FORMAT);
	m_synDepthWithHole = new Image<DepthType>(height, width, DEPTHMAP_CHROMA_FORMAT);
	m_synHoles = new Image<HoleType>(height, width, HOLE_CHROMA_FORMAT);
	m_synImageWithHole->initMat(width, height, 3);
	m_synDepthWithHole->initMat(width, height, 1);
	m_synHoles->initMat(width, height, 1);

	m_fillableHoles = new Image<HoleType>();
	m_fillableHoles->initMat(width, height, MASK_CHANNELS);
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

bool View::init(uint indexSyn)
{
	m_indexView = indexSyn;

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
	m_synImage = new Image<ImageType>();
	m_synDepth = new Image<DepthType>();
	m_synFills = new Image<ImageType>();
	m_synImage->initMat(sw, sh, IMAGE_CHANNELS);
	m_synDepth->initMat(sw, sh, DEPTH_CHANNELS);
	m_synFills->initMat(sw, sh, 1);
	//m_synHoles->initMat(sw, sh, 1);
	m_imgBound.initMat(sw, sh, 1);
	m_imgMask[0].initMat(sw, sh, MASK_CHANNELS);
	m_imgMask[1].initMat(sw, sh, MASK_CHANNELS);

	if (!m_synImage->initYUVFromMat(m_synFills->getMat()->widthStep)) return false;
	if (!m_synDepth->initYFromMat()) return false;
	if (!m_synFills->initYFromMat()) return false;

	return true;
}

bool View::init_3Dwarp()
{
	cam.init(m_indexView);

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

		if (cfg.getDepthType() == cfg.DEPTH_FROM_CAMERA)
			computeDepthFromCam(i, distance);
		else if (cfg.getDepthType() == cfg.DEPTH_FROM_ORIGIN)
			computeDepthFromSpace(i, distance);
		else
			return false;
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

bool View::synthesizeView()
{
	ImageType*** pImage = m_image->getData();
	DepthType** pDepthMap = m_depth->Y;

	m_depthSynthesis->apply(pDepthMap);
	m_viewSynthesisReverse->apply(pImage);

	// copy warping results for BNR
	m_synImageWithHole->convertMatToBuffer1D(m_synImage, 3);
	m_synDepthWithHole->convertMatToBuffer1D(m_synDepth, 1);
	m_synHoles->convertMatToBuffer1D(m_synHoles, 1);

	return true;
}