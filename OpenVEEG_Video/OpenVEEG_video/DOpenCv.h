#ifndef DOPENCV_H_
#define DOPENCV_H_

#include "DeckLinkAPI_h.h"

#include <opencv2/core/core.hpp>

#include "ComPtr.h"

bool VideoFrameToMat(ComPtr<IDeckLinkVideoInputFrame> in,
	cv::Mat& out);

#endif /* DECKLINKOPENCV_H_ */