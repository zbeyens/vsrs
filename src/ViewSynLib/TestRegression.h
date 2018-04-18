#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "InputStream.h"

class TestRegression
{
public:
	TestRegression()
	{
	}

	~TestRegression()
	{
	}

	void apply(unique_ptr<Image<ImageType>>& output, int frameNo);

private:
	ConfigSyn & cfg = ConfigSyn::getInstance();

};

