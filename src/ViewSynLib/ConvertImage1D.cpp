#include "ConvertImage1D.h"

ConvertImage1D::ConvertImage1D(ViewSynthesis1D* viewSynthesis1D, int height, int width)
	: ConvertImage(height, width)
{
	m_viewSynthesis1D = viewSynthesis1D;
}

void ConvertImage1D::apply(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight)
{
	ImageType* tar_L, *tar_R;

	// Left Synthesized Image
	tar_L = pRefLeft.getBuffer1D();
	memcpy(&tar_L[m_height*m_width * 0], m_viewSynthesis1D->GetSynColorLeftY(), m_height*m_width);
	memcpy(&tar_L[m_height*m_width * 1], m_viewSynthesis1D->GetSynColorLeftU(), m_height*m_width);
	memcpy(&tar_L[m_height*m_width * 2], m_viewSynthesis1D->GetSynColorLeftV(), m_height*m_width);

	// Right Synthesized Image
	tar_R = pRefRight.getBuffer1D();
	memcpy(&tar_R[m_height*m_width * 0], m_viewSynthesis1D->GetSynColorRightY(), m_height*m_width);
	memcpy(&tar_R[m_height*m_width * 1], m_viewSynthesis1D->GetSynColorRightU(), m_height*m_width);
	memcpy(&tar_R[m_height*m_width * 2], m_viewSynthesis1D->GetSynColorRightV(), m_height*m_width);

	// Left Synthesized Depth Image
	memcpy(pRefDepthLeft.getBuffer1D(), m_viewSynthesis1D->GetSynDepthLeft(), m_height*m_width);

	// Right Synthesized Depth Image
	memcpy(pRefDepthRight.getBuffer1D(), m_viewSynthesis1D->GetSynDepthRight(), m_height*m_width);
}
