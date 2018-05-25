#include "View.h"

View::View()
{
	m_matH_R2V = m_matH_V2R = NULL;

	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	m_imageNotUpsampled = new Image<ImageType>();
	m_imageHorUpsampled = new Image<ImageType>();
	m_image = new Image<ImageType>();
	m_depth = new Image<DepthType>();

	m_synHoles = new Image<HoleType>(sh, sw, HOLE_CHROMA_FORMAT);
	m_synHoles->initMat(sw, sh, 1);

	m_fillableHoles = new Image<HoleType>();
	m_fillableHoles->initMat(sw, sh, MASK_CHANNELS);
}

View::~View()
{
	xReleaseMemory();
}

void View::xReleaseMemory()
{
	Tools::safeReleaseMat(m_matH_R2V);
	Tools::safeReleaseMat(m_matH_V2R);
}

bool View::init(uint indexSyn)
{
	m_indexView = indexSyn;

	if (!initImages()) return false;

	init_3Dwarp();

	return true;
}

bool View::initImages()
{
	int sw = cfg.getSourceWidth();
	int sh = cfg.getSourceHeight();

	m_imageNotUpsampled->initYUV(sh, sw, IMAGE_CHROMA_FORMAT);
	m_imageHorUpsampled->initYUV(sh, sw * cfg.getPrecision(), IMAGE_CHROMA_FORMAT);

	m_image->initYUV(sh, sw * cfg.getPrecision(), IMAGE_CHROMA_FORMAT); // width should be sw if no upsampling (1D?)
	m_depth->initYUV(sh, sw, DEPTHMAP_CHROMA_FORMAT);

	//create images
	m_synImage = new Image<ImageType>(sh, sw, IMAGE_CHROMA_FORMAT);
	m_synDepth = new Image<DepthType>(sh, sw, DEPTHMAP_CHROMA_FORMAT);
	m_synFills = new Image<ImageType>();
	m_imgMask[0] = new Image<HoleType>();
	m_imgMask[1] = new Image<HoleType>();
	m_synImage->initMat(sw, sh, IMAGE_CHANNELS);
	m_synDepth->initMat(sw, sh, DEPTH_CHANNELS);
	m_synFills->initMat(sw, sh, 1);
	m_imgMask[0]->initMat(sw, sh, MASK_CHANNELS);
	m_imgMask[1]->initMat(sw, sh, MASK_CHANNELS);

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

bool View::convertMatToBuffer1D()
{
	m_synImage->convertMatToBuffer1D(m_synImage, 3);
	m_synDepth->convertMatToBuffer1D(m_synDepth, 1);
	m_synHoles->convertMatToBuffer1D(m_synHoles, 1);

	return true;
}