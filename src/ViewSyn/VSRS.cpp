/*
* View Synthesis Reference Software
* ****************************************************************************
* Copyright (c) 2011/2013 Poznan University of Technology
*
* Address:
*   Poznan University of Technology,
*   Polanka 3 Street, Poznan, Poland
*
* Authors:
*   Krysztof Wegner     <kwegner@multimedia.edu.pl>
*   Olgierd Stankiewicz <ostank@multimedia.edu.pl>
*
* You may use this Software for any non-commercial purpose, subject to the
* restrictions in this license. Some purposes which can be non-commercial are
* teaching, academic research, and personal experimentation. You may also
* distribute this Software with books or other teaching materials, or publish
* the Software on websites, that are intended to teach the use of the
* Software.
*
* Reference to the following source document:
*
* Takanori Senoh, Kenji Yamamoto, Nobuji Tetsutani, Hiroshi Yasuda, Krzysztof Wegner,
* "View Synthesis Reference Software (VSRS) 4.2 with improved inpainting and hole filing"
* ISO/IEC JTC1/SC29/WG11 MPEG2017/M40xx April 2017, Hobart, Australia
*
* are required in all documents that report any usage of the software.

* You may not use or distribute this Software or any derivative works in any
* form for commercial purposes. Examples of commercial purposes would be
* running business operations, licensing, leasing, or selling the Software, or
* distributing the Software for use with commercial products.
* ****************************************************************************
*/

#include "Clock.h"
#include "ConfigSyn.h"
#include "Parser.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "PipelineFactory.h"
#include "ViewSynthesis.h"
#include "TestRegression.h"

int main(int argc, char *argv[])
{
	if (argc < 2) return 0;

	// Parse config file
	Parser parser;
	parser.setFilename(argv[1]);
	parser.setCommands(argc, argv);
	parser.parse();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	// Init synthesis pipeline
	PipelineFactory pipelineFactory;
	unique_ptr<ViewSynthesis> viewSynthesis(pipelineFactory.createPipeline());

	shared_ptr<Image<ImageType>> imageBuffer(new Image<ImageType>(cfg.getSourceHeight(), cfg.getSourceWidth(), BUFFER_CHROMA_FORMAT)); // store images to upsample one by one

	Clock clock;

	cout << "View Synthesis Reference Software (VSRS), Version " << VERSION << endl;
	cout << "     - MPEG-I Visual, April 2017" << endl << endl;

	if (!viewSynthesis->init()) return 0;

	// Open input and output files
	vector<InputStream> inImages;
	vector<InputStream> inDepths;

	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		inImages.push_back(InputStream(cfg.getViewImageName(i)));
		inDepths.push_back(InputStream(cfg.getDepthMapName(i)));
	}

	OutputStream outImage(cfg.getOutputVirViewImageName());
	OutputStream outDepth(cfg.getOutputVirDepthMapName());

	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		inImages[i].openRB();
		inDepths[i].openRB();
	}
	outImage.openWB();
	outDepth.openWB();

	clock.setStartTime();

	uint n;
	for (n = cfg.getStartFrame(); n < cfg.getStartFrame() + cfg.getNumberOfFrames(); n++)
	{
		cout << "frame number = " << n;
		viewSynthesis->setFrameNumber(n - cfg.getStartFrame());

		for (size_t i = 0; i < cfg.getNViews(); i++)
		{
			inDepths[i].readOneFrame(viewSynthesis->getView(i)->getDepth()->getFrame(), viewSynthesis->getView(i)->getDepth()->getSize(), n);
			inImages[i].readOneFrame(imageBuffer->getFrame(), imageBuffer->getSize(), n);
			viewSynthesis->setInputImage(i, imageBuffer);

			cout << ".";
		}

		if (!viewSynthesis->apply(imageBuffer)) break;

		cout << ".";

		outImage.writeOneFrame(imageBuffer->getFrame(), imageBuffer->getSize());
		//outDepth.writeOneFrame(viewSynthesis->getSynDepth()->getFrame(), viewSynthesis->getView(0)->getDepth()->getSize());


		clock.setEndTime();

		if (cfg.getTesting() == cfg.TESTING_ENABLED)
		{
			TestRegression testRegression;
			testRegression.apply(imageBuffer, n);
		}
	}

	for (size_t i = 0; i < cfg.getNViews(); i++)
	{
		inImages[i].close();
		inDepths[i].close();
	}
	outImage.close();

	clock.setTotalTime();

	return 0;
}