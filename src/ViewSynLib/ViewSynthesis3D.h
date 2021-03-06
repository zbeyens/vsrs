#ifndef _VIEW_SYNTHESIS_H_
#define _VIEW_SYNTHESIS_H_

#include "ViewSynthesis.h"
#include "BlendingHoles.h"
#include "Blending.h"
#include "Inpaint.h"

/*!
*	View Synthesis pipeline using general mode
*/
class ViewSynthesis3D : public ViewSynthesis
{
public:
	ViewSynthesis3D();
	~ViewSynthesis3D();

	/*!
		Init views, compute their weights and init result images
	*/
	bool init();

	/*!
		General pipeline.
	*/
	bool  apply(shared_ptr<Image<ImageType>> outImg);

private:
	/*!
		Compute weights from the baseline of each view
	*/
	void computeWeights();
	/*!
		Create all the images for synthesis
	*/
	void initResultImages();

	ConfigSyn& cfg;

	shared_ptr<Image<ImageType>> m_holesMask;	//!> holes not fillable by any view -> to inpaint
	shared_ptr<Image<ImageType>> m_blendedImage;          //!> image after blending
	shared_ptr<Image<ImageType>> m_synImage;     //!> synthesized image after blending
};

#endif
