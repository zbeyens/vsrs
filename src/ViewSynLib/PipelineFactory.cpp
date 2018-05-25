#include "PipelineFactory.h"

PipelineFactory::PipelineFactory()
	: cfg(ConfigSyn::getInstance())
{
}

ViewSynthesis* PipelineFactory::createPipeline()
{
	ConfigSyn& cfg = ConfigSyn::getInstance();

	if (cfg.getSynthesisMode() == cfg.MODE_3D)
	{
		if (cfg.getBoundaryNoiseRemoval() == cfg.BNR_DISABLED)
			return new ViewSynthesis3D();
		else
			return new BoundaryNoiseRemoval3D();
	}
	else if (cfg.getSynthesisMode() == cfg.MODE_1D)
		return new ViewSynthesis1D();
}