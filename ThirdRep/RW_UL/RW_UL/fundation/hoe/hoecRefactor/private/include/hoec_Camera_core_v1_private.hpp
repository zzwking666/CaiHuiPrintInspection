#pragma once
#include "DVPCamera.h"

struct _MV_FRAME_OUT_;
typedef struct _MV_FRAME_OUT_ MV_FRAME_OUT;
struct _MV_FRAME_OUT_INFO_EX_;
typedef struct _MV_FRAME_OUT_INFO_EX_ MV_FRAME_OUT_INFO_EX;

namespace cv {
	class Mat;
}

namespace rw
{
	namespace hoec_v1
	{
		struct ImageFrameConvert
		{
			static cv::Mat MVS_ConvertFrameToMat(const MV_FRAME_OUT& frameInfo);
			static cv::Mat MVS_ConvertFrameToMat(const MV_FRAME_OUT_INFO_EX& frameInfo, unsigned char* pData);
			static cv::Mat DS_ConvertFrameToMat(const dvpFrame& frame, void* pBuffer);
		};
	}
}