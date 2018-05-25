#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"

#include "ViewSynthesis3D.h"
#include "ViewSynthesis1D.h"
#include "BoundaryNoiseRemoval3D.h"
#include "BoundaryNoiseRemoval1D.h"

class PipelineFactory
{
public:
	PipelineFactory();

	~PipelineFactory() {}

	ViewSynthesis* createPipeline();

private:
	ConfigSyn & cfg;
};