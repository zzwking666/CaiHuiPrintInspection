#include"rqw_CameraObjectCore.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include"hoec_CameraFactory_v1.hpp"
#include"hoec_Camera_v1.hpp"

namespace rw
{
	namespace rqw
	{
		QImage cvMatToQImage(const cv::Mat& mat)
		{
			QImage result;
			auto type = mat.type();
			if (type == CV_8UC1) {
				cv::Mat colorMat;
				cv::cvtColor(mat, colorMat, cv::COLOR_GRAY2BGR);
				result = QImage(colorMat.data, colorMat.cols, colorMat.rows, colorMat.step[0], QImage::Format_RGB888).rgbSwapped();
			}
			else if (type == CV_8UC3) {
				result = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGB888).rgbSwapped();
			}
			else if (type == CV_8UC4) {
				result = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGBA8888);
			}
			else if (type == CV_16UC1) {
				cv::Mat temp8u;
				double minVal, maxVal;
				cv::minMaxLoc(mat, &minVal, &maxVal);
				if (maxVal > minVal) {
					mat.convertTo(temp8u, CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
				}
				else {
					temp8u = cv::Mat::zeros(mat.size(), CV_8UC1);
				}
				result = QImage(temp8u.data, temp8u.cols, temp8u.rows, temp8u.step[0], QImage::Format_Grayscale8).copy();
			}
			else if (type == CV_16UC3) {
				std::vector<cv::Mat> channels(3), channels8u(3);
				cv::split(mat, channels);
				double minVal, maxVal;
				for (int i = 0; i < 3; ++i) {
					cv::minMaxLoc(channels[i], &minVal, &maxVal);
					if (maxVal > minVal) {
						channels[i].convertTo(channels8u[i], CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
					}
					else {
						channels8u[i] = cv::Mat::zeros(channels[i].size(), CV_8UC1);
					}
				}
				cv::Mat temp8u;
				cv::merge(channels8u, temp8u);
				result = QImage(temp8u.data, temp8u.cols, temp8u.rows, temp8u.step[0], QImage::Format_RGB888).rgbSwapped().copy();
			}
			else if (type == CV_32FC1) {
				cv::Mat temp8u;
				double minVal, maxVal;
				cv::minMaxLoc(mat, &minVal, &maxVal);
				if (maxVal > minVal) {
					mat.convertTo(temp8u, CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
				}
				else {
					temp8u = cv::Mat::zeros(mat.size(), CV_8UC1);
				}
				result = QImage(temp8u.data, temp8u.cols, temp8u.rows, temp8u.step[0], QImage::Format_Grayscale8).copy();
			}
			else if (type == CV_32FC3) {
				std::vector<cv::Mat> channels(3), channels8u(3);
				cv::split(mat, channels);
				double minVal, maxVal;
				for (int i = 0; i < 3; ++i) {
					cv::minMaxLoc(channels[i], &minVal, &maxVal);
					if (maxVal > minVal) {
						channels[i].convertTo(channels8u[i], CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
					}
					else {
						channels8u[i] = cv::Mat::zeros(channels[i].size(), CV_8UC1);
					}
				}
				cv::Mat temp8u;
				cv::merge(channels8u, temp8u);
				result = QImage(temp8u.data, temp8u.cols, temp8u.rows, temp8u.step[0], QImage::Format_RGB888).rgbSwapped().copy();
			}
			else {
				result = QImage();
			}

			return result;
		}

		QVector<CameraMetaData> CheckCameraList(CameraProvider provider)
		{
			std::vector<hoec_v1::CameraIP> stdCameraIpList;
			if (provider == CameraProvider::MVS)
			{
				stdCameraIpList = hoec_v1::CameraFactory::checkAllCamera(hoec_v1::CameraProvider::MVS);
			}
			else if (provider == CameraProvider::DS)
			{
				stdCameraIpList = hoec_v1::CameraFactory::checkAllCamera(hoec_v1::CameraProvider::DS);
			}

			QVector<CameraMetaData> cameraIpList;
			for (auto& cameraIp : stdCameraIpList)
			{
				CameraMetaData cameraMetaData;
				cameraMetaData.ip = QString::fromStdString(cameraIp.ip);
				cameraMetaData.provider = QString::fromStdString(hoec_v1::to_string(cameraIp.provider));
				cameraIpList.push_back(cameraMetaData);
			}

			return cameraIpList;
		}
	} // namespace rqw
} // namespace rw