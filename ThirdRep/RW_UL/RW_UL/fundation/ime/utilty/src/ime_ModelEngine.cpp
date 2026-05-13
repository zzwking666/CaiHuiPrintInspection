#include"ime_ModelEngine.h"

namespace rw
{
	void ModelEngine::setDrawStatus(bool status)
	{
		_isDraw = status;
	}

	cv::Mat ModelEngine::draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList)
	{
		return mat;
	}

	std::vector<DetectionRectangleInfo> ModelEngine::processImg(const cv::Mat& mat)
	{
		preprocess(mat);
		infer();
		std::vector<DetectionRectangleInfo> detection = postProcess();
		return detection;
	}

	cv::Mat ModelEngine::processImg(const cv::Mat& mat, std::vector<DetectionRectangleInfo>& detection)
	{
		preprocess(mat);
		infer();
		detection = postProcess();
		if (_isDraw)
		{
			return draw(mat, detection);
		}
		return mat;
	}
}