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
	if (cfg.getViewBlending() == cfg.BLENDING_HOLES_CLOSER)
		return new BlendingCloser();
	else if (cfg.getViewBlending() == cfg.BLENDING_HOLES_WEIGHTED)
		return new BlendingWeighted();

	cout << endl << "Warning: the ViewBlending parameter value does not associate to any algorithm. Returning default algorithm.";
	return new BlendingWeighted();
}

Blending* AlgoFactory::createBlending()
{
	if (cfg.getBlendingMode() == cfg.BLENDING_MODE_2V)
		return new Blending2V();

	cout << endl << "Warning: the BlendingMode parameter value does not associate to any algorithm. Returning default algorithm.";
	return new Blending2V();
}

Inpaint* AlgoFactory::createInpaint()
{
	if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEFAULT)
		return new InpaintDefault();
	else if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEPTH_BASED)
		return new InpaintDepthBased();

	cout << endl << "Warning: the IvsrsInpaint parameter value does not associate to any algorithm. Returning default algorithm.";
	return new InpaintDefault();
}