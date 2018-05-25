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
#include "FilterHorizontalCubicOct2D.h"
#include "FilterVerticalLinearHalf2D.h"
#include "FilterVerticalLinearQuarter2D.h"
#include "FilterVerticalCubicHalf2D.h"
#include "FilterVerticalCubicQuarter2D.h"
#include "FilterVerticalCubicOct2D.h"
#include "FilterVerticalAVCHalf2D.h"
#include "FilterVerticalAVCQuarter2D.h"
#include "InpaintDepthBased.h"
#include "InpaintDefault.h"
#include "BlendingWeighted.h"
#include "BlendingCloser.h"
#include "WarpIpelDepth.h"
#include "WarpDepth.h"
#include "WarpViewReverse.h"
#include "WarpIpelViewReverse.h"
#include "Blending2V.h"

class AlgoFactory
{
public:
	AlgoFactory();

	~AlgoFactory() {}

	template<typename PixelType>
	Filter<PixelType>* createFilter(bool horizontal);

	Warp* createWarpDepth();

	Warp* createWarpViewReverse();

	BlendingHoles* createBlendingHoles();

	Blending* createBlending();

	Inpaint* createInpaint();

private:
	ConfigSyn & cfg;
};

template<typename PixelType>
Filter<PixelType>* AlgoFactory::createFilter(bool horizontal)
{
	switch (cfg.getPrecision())
	{
	case 1: // INTEGER PEL
		return new FilterDummy2D<PixelType>();
	case 2: // HALF PEL
		if (cfg.getFilter() == cfg.FILTER_LINEAR)
			if (horizontal) return new FilterHorizontalLinearHalf2D<PixelType>();
			else return new FilterVerticalLinearHalf2D<PixelType>();
		else if (cfg.getFilter() == cfg.FILTER_CUBIC)
			if (horizontal) return new FilterHorizontalCubicHalf2D<PixelType>();
			else return new FilterVerticalCubicHalf2D<PixelType>();
		else if (cfg.getFilter() == cfg.FILTER_AVC)
			if (horizontal) return new FilterHorizontalAVCHalf2D<PixelType>();
			else return new FilterVerticalAVCHalf2D<PixelType>();
	case 4: // QUARTER PEL
		if (cfg.getFilter() == cfg.FILTER_LINEAR)
			if (horizontal) return new FilterHorizontalLinearQuarter2D<PixelType>();
			else return new FilterVerticalLinearQuarter2D<PixelType>();
		else if (cfg.getFilter() == cfg.FILTER_CUBIC)
			if (horizontal) return new FilterHorizontalCubicQuarter2D<PixelType>();
			else return new FilterVerticalCubicQuarter2D<PixelType>();
		else if (cfg.getFilter() == cfg.FILTER_AVC)
			if (horizontal) return new FilterHorizontalAVCQuarter2D<PixelType>();
			else return new FilterVerticalAVCQuarter2D<PixelType>();
	case 8: // OCTO PEL
		if (cfg.getFilter() == cfg.FILTER_LINEAR) {
			cout << "Octo-pel with linear filter is not yet supported" << endl;
			return false;
		}
		else if (cfg.getFilter() == cfg.FILTER_CUBIC)
			if (horizontal) return new FilterHorizontalCubicOct2D<PixelType>();
			else return new FilterVerticalCubicOct2D<PixelType>();
		else if (cfg.getFilter() == cfg.FILTER_AVC) {
			cout << "Octo-pel with AVC filter is not yet supported" << endl;
			return false;
		}
	default:
		return false;
		break;
	}
}
