#include "TestRegression.h"

void TestRegression::apply(unique_ptr<Image<ImageType>>& output, int frameNo)
{
	InputStream inTestImage(cfg.getTestImageName());
	inTestImage.openRB();

	Image<ImageType> testImage(cfg.getSourceHeight(), cfg.getSourceWidth(), BUFFER_CHROMA_FORMAT); // store images to upsample one by one

	inTestImage.readOneFrame(testImage.getFrame(), testImage.getSize(), frameNo);

	string result = "Regression Test Passed";
	for (size_t i = 0; i < testImage.getSize(); i++)
	{
		if (testImage.getFrame()[i] != output->getFrame()[i])
		{
			result = "Regression Test Failed";
			break;
		}
	}
	cout << result << endl;

	inTestImage.close();
}
