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
//#include <string.h>
#include <time.h>

#include "ConfigSyn.h"
#include "Parser.h"
#include "yuv.h"
#include "ViewSynthesis.h"

int main(int argc, char *argv[])
{
	if (argc < 2) return 0;

	Parser& parser = Parser::getInstance();
	parser.setFilename(argv[1]);
	parser.setCommands(argc, argv);
	parser.parse();

	ConfigSyn& cfg = ConfigSyn::getInstance();

	ViewSynthesis viewSynthesis;
	CIYuv<ImageType> yuvBuffer;

#ifdef OUTPUT_COMPUTATIONAL_TIME
	clock_t start, finish, first;
	first = start = clock();
#endif

	cout << "View Synthesis Reference Software (VSRS), Version " << VERSION << endl;
	cout << "     - MPEG-I Visual, April 2017" << endl << endl;

	if (!viewSynthesis.Init()) return 10;

	if (!yuvBuffer.resize(cfg.getSourceHeight(), cfg.getSourceWidth(), 420)) return 2;

	FILE* fin_view_l = fopen(cfg.getLeftViewImageName().c_str(), "rb");
	FILE* fin_view_r = fopen(cfg.getRightViewImageName().c_str(), "rb");
	FILE* fin_depth_l = fopen(cfg.getLeftDepthMapName().c_str(), "rb");
	FILE* fin_depth_r = fopen(cfg.getRightDepthMapName().c_str(), "rb");
	FILE* fout = fopen(cfg.getOutputVirViewImageName().c_str(), "wb");

	if (fin_view_l == NULL || fin_view_r == NULL || fin_depth_l == NULL || fin_depth_r == NULL || fout == NULL)
	{
		cout << "Can't open input file(s)" << endl;
		return 3;
	}

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	cout << "Initialization: " << (double)(finish - start) / CLOCKS_PER_SEC << " sec" << endl;
	start = finish;
#endif

	uint n;
	for (n = cfg.getStartFrame(); n < cfg.getStartFrame() + cfg.getNumberOfFrames(); n++)
	{
		cout << "frame number = " << n;

		if (!viewSynthesis.getDepthBufferLeft()->readOneFrame(fin_depth_l, n) ||
			!viewSynthesis.getDepthBufferRight()->readOneFrame(fin_depth_r, n)) break;
		cout << ".";

		viewSynthesis.setFrameNumber(n - cfg.getStartFrame()); // Zhejiang

		if (!yuvBuffer.readOneFrame(fin_view_l, n)) break;
		if (!viewSynthesis.SetReferenceImage(1, &yuvBuffer)) break;
		cout << ".";

		if (!yuvBuffer.readOneFrame(fin_view_r, n)) break;
		if (!viewSynthesis.SetReferenceImage(0, &yuvBuffer)) break;
		cout << ".";

		if (!viewSynthesis.DoViewInterpolation(&yuvBuffer)) break;
		cout << ".";

		if (!yuvBuffer.writeOneFrame(fout)) break;

#ifdef OUTPUT_COMPUTATIONAL_TIME
		finish = clock();
		cout << "->End (" << (double)(finish - start) / CLOCKS_PER_SEC << " sec)" << endl;
		start = finish;
#else
		cout << "->End" << endl;
#endif
	}

	if (fin_view_l && fin_view_r && fin_depth_l && fin_depth_r && fout)
	{
		cout << "end" << endl;
		fclose(fout);
		fclose(fin_view_l);
		fclose(fin_view_r);
		fclose(fin_depth_l);
		fclose(fin_depth_r);
	}

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	cout << "Total: " << ((double)(finish - first)) / ((double)CLOCKS_PER_SEC) << " sec" << endl;
#endif


	return 0;
}