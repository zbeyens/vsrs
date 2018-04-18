#ifndef _VIEW_SYNTHESIS_H_
#define _VIEW_SYNTHESIS_H_

#include "ViewSynthesis.h"
#include "BlendingHoles.h"
#include "Blending.h"
#include "Inpaint.h"
#include "BoundaryNoiseRemoval3D.h"

/**
* View Synthesis using general mode
*/
class ViewSynthesis3D : public ViewSynthesis
{
public:
	ViewSynthesis3D();
	~ViewSynthesis3D();

	void    xReleaseMemory();

	/**
	  Init Left and Right View
	*/
	bool init();

	bool  apply(unique_ptr<Image<ImageType>>& pSynYuvBuffer);

	View* getViewLeft() { return m_views[0]; }
	View* getViewRight() { return m_views[1]; }

private:

	void computeWeights(); //!> Compute weight left and right from baseline left and right
	void initResultImages();	//!> Create all the images for synthesis

	void initMasks(); //!> pixels holes (or unstable pixels) which will be replaced by pixels synthesized from right view

	ConfigSyn& cfg;

	double m_totalBaseline;

	Image<ImageType>* m_blendedImage;          //!> Blended image
	Image<DepthType>* m_blendedDepth;     //!> Blended depth
	Image<ImageType>* m_synImage; //!> The final image buffer to be output (blended image with inpainting)
	Image<ImageType>* m_holesMask;	//!> holes not fillable by any view -> to inpaint
};

#endif
