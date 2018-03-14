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
#include "ImageData.h"
#include "Application.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "VSRS.h"

int main(int argc, char *argv[])
{
	if (argc < 2) return 0;

	// Parse config file
	Parser& parser = Parser::getInstance();
	parser.setFilename(argv[1]);
	parser.setCommands(argc, argv);
	parser.parse();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	// Init synthesis
	Application app;
	ImageData<ImageType> imageBuffer(cfg.getSourceHeight(), cfg.getSourceWidth(), BUFFER_CHROMA_FORMAT); // store images to upsample one by one

	Clock clock;

	cout << "View Synthesis Reference Software (VSRS), Version " << VERSION << endl;
	cout << "     - MPEG-I Visual, April 2017" << endl << endl;

	if (!app.init()) return 10;

	// Open input and output files
	InputStream inViewLeft(cfg.getLeftViewImageName()),
		inViewRight(cfg.getRightViewImageName()),
		inDepthLeft(cfg.getLeftDepthMapName()),
		inDepthRight(cfg.getRightDepthMapName());

	OutputStream outSynthesizedImage(cfg.getOutputVirViewImageName());

	inViewLeft.openRB();
	inViewRight.openRB();
	inDepthLeft.openRB();
	inDepthRight.openRB();
	outSynthesizedImage.openWB();

	clock.init();

	uint n;
	for (n = cfg.getStartFrame(); n < cfg.getStartFrame() + cfg.getNumberOfFrames(); n++)
	{
		cout << "frame number = " << n;

		inDepthLeft.readOneFrame(app.getDepthMapLeft().getFrame(), app.getDepthMapLeft().getSize(), n);
		inDepthRight.readOneFrame(app.getDepthMapRight().getFrame(), app.getDepthMapRight().getSize(), n);
		cout << ".";

		app.setFrameNumber(n - cfg.getStartFrame());

		inViewLeft.readOneFrame(imageBuffer.getFrame(), imageBuffer.getSize(), n);
		if (!app.upsampleImage(1, &imageBuffer)) break;
		cout << ".";

		inViewRight.readOneFrame(imageBuffer.getFrame(), imageBuffer.getSize(), n);
		if (!app.upsampleImage(0, &imageBuffer)) break;
		cout << ".";

		if (!app.DoViewInterpolation(imageBuffer)) break;
		cout << ".";

		outSynthesizedImage.writeOneFrame(imageBuffer.getFrame(), imageBuffer.getSize());

		clock.getEndTime();
	}

	cout << "end" << endl;
	outSynthesizedImage.close();
	inViewLeft.close();
	inViewRight.close();
	inDepthLeft.close();
	inDepthRight.close();

	clock.getTotalTime();

	return 0;
}