#pragma once

#include "Image.h"
#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "InputStream.h"

/*!
	Regression testing consists to check that the changes of the software have not included new mistakes.
*/
class TestRegression
{
public:
	TestRegression()
	{
	}

	~TestRegression()
	{
	}

	/*!
		It checks whether an image is the same than before the changes.
	*/
	void apply(shared_ptr<Image<ImageType>> output, int frameNo);

private:
	ConfigSyn & cfg = ConfigSyn::getInstance();

};

