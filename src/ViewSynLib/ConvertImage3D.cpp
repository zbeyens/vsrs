#include "ConvertImage3D.h"

ConvertImage3D::ConvertImage3D(ViewSynthesis3D* viewSynthesis3D, int height, int width)
	: ConvertImage(height, width)
{
	m_viewSynthesis3D = viewSynthesis3D;
}

void ConvertImage3D::apply(ImageData<ImageType>& pRefLeft, ImageData<ImageType>& pRefRight, ImageData<DepthType>& pRefDepthLeft, ImageData<DepthType>& pRefDepthRight, ImageData<HoleType>& pRefHoleLeft, ImageData<HoleType>& pRefHoleRight)
{
	ImageType* tar_L_image, *tar_R_image;
	ImageType* org_L_image, *org_R_image;
	DepthType* tar_L_depth, *tar_R_depth;
	DepthType* org_L_depth, *org_R_depth;
	HoleType* tar_L_hole, *tar_R_hole;
	HoleType* org_L_hole, *org_R_hole;

	// Color Image with hole
	org_L_image = (ImageType*)m_viewSynthesis3D->GetSynLeftWithHole()->imageData;
	org_R_image = (ImageType*)m_viewSynthesis3D->GetSynRightWithHole()->imageData;
	tar_L_image = pRefLeft.getBuffer1D();
	tar_R_image = pRefRight.getBuffer1D();
	memcpy(tar_L_image, org_L_image, m_height*m_width * 3 * sizeof(ImageType));
	memcpy(tar_R_image, org_R_image, m_height*m_width * 3 * sizeof(ImageType));

	org_L_depth = (DepthType*)m_viewSynthesis3D->GetSynDepthLeftWithHole()->imageData;
	org_R_depth = (DepthType*)m_viewSynthesis3D->GetSynDepthRightWithHole()->imageData;
	tar_L_depth = pRefDepthLeft.getBuffer1D();
	tar_R_depth = pRefDepthRight.getBuffer1D();
	memcpy(tar_L_depth, org_L_depth, m_height*m_width * sizeof(DepthType));
	memcpy(tar_R_depth, org_R_depth, m_height*m_width * sizeof(DepthType));

	org_L_hole = (HoleType*)m_viewSynthesis3D->GetSynHoleLeft()->imageData;
	org_R_hole = (HoleType*)m_viewSynthesis3D->GetSynHoleRight()->imageData;
	tar_L_hole = pRefHoleLeft.getBuffer1D();
	tar_R_hole = pRefHoleRight.getBuffer1D();
	memcpy(tar_L_hole, org_L_hole, m_height*m_width * sizeof(HoleType));
	memcpy(tar_R_hole, org_R_hole, m_height*m_width * sizeof(HoleType));

	cout << "hello1" << endl;

}
