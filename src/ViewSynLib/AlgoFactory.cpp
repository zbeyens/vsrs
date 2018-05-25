#include "AlgoFactory.h"

AlgoFactory::AlgoFactory()
	: cfg(ConfigSyn::getInstance())
{
}

Warp* AlgoFactory::createWarpDepth()
{
	if (cfg.getPrecision() == 1)
		//return new WarpIpelDepth(); //does not work for 16 BitDepth
		return new WarpDepth();
	else
		return new WarpDepth();
}

Warp* AlgoFactory::createWarpViewReverse()
{
	if (cfg.getPrecision() == 1)
		return new WarpIpelViewReverse();
	else
		return new WarpViewReverse();
}

BlendingHoles* AlgoFactory::createBlendingHoles()
{
	if (cfg.getViewBlending() == cfg.BLEND_CLOSER)
		return new BlendingCloser();
	else if (cfg.getViewBlending() == cfg.BLEND_WEIGHTED)
		return new BlendingWeighted();
}

Blending* AlgoFactory::createBlending()
{
	return new Blending2V();
}

Inpaint* AlgoFactory::createInpaint()
{
	if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEFAULT)
		return new InpaintDefault();
	else if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEPTH_BASED)
		return new InpaintDepthBased();
}