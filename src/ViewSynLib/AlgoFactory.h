#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"

#include "FilterDummy2D.h"
#include "FilterHorizontalAVCHalf2D.h"
#include "FilterHorizontalAVCQuarter2D.h"
#include "FilterHorizontalCubicHalf2D.h"
#include "FilterHorizontalCubicQuarter2D.h"
#include "FilterHorizontalLinearHalf2D.h"
#include "FilterHorizontalLinearQuarter2D.h"
#include "InpaintAdvanced.h"
#include "InpaintDefault.h"
#include "BlendingAll.h"
#include "BlendingCloser.h"

class AlgoFactory
{
public:
	AlgoFactory()
	{
	}

	~AlgoFactory()
	{
	}

	static Inpaint* createInpaint()
	{
		ConfigSyn & cfg = ConfigSyn::getInstance();

		if (cfg.getIvsrsInpaint() == cfg.INPAINT_DEFAULT)
			return new InpaintDefault();
		else if (cfg.getIvsrsInpaint() == cfg.INPAINT_ADVANCED)
			return new InpaintAdvanced();
	}

	static BlendingHoles* createBlendingHoles()
	{
		ConfigSyn & cfg = ConfigSyn::getInstance();

		if (cfg.getViewBlending() == cfg.BLEND_CLOSER)
			return new BlendingCloser();
		else if (cfg.getViewBlending() == cfg.BLEND_ALL)
			return new BlendingAll();
	}

	template<typename PixelType>
	Filter<PixelType>* createFilter()
	{
		ConfigSyn & cfg = ConfigSyn::getInstance();

		int precision;
		if (cfg.getSynthesisMode() == cfg.MODE_3D) // General mode
			precision = cfg.getPrecision();
		else if (cfg.getSynthesisMode() == cfg.MODE_1D) // 1D mode: No upsampling is done before going into Visbd
			precision = 1;

		switch (precision)
		{
			//todo precision 1 not working!
		case 1: // INTEGER PEL
			return new FilterDummy2D<PixelType>();
		case 2: // HALF PEL
			if (cfg.getFilter() == cfg.FILTER_LINEAR)
				return new FilterHorizontalLinearHalf2D<PixelType>();
			else if (cfg.getFilter() == cfg.FILTER_CUBIC)
				return new FilterHorizontalCubicHalf2D<PixelType>();
			else if (cfg.getFilter() == cfg.FILTER_AVC)
				return new FilterHorizontalAVCHalf2D<PixelType>();
		case 4: // QUARTER PEL
			if (cfg.getFilter() == cfg.FILTER_LINEAR)
				return new FilterHorizontalLinearQuarter2D<PixelType>();
			else if (cfg.getFilter() == cfg.FILTER_CUBIC)
				return new FilterHorizontalCubicQuarter2D<PixelType>();
			else if (cfg.getFilter() == cfg.FILTER_AVC)
				return new FilterHorizontalAVCQuarter2D<PixelType>();
		default:
			break;
		}
	}

private:
};
