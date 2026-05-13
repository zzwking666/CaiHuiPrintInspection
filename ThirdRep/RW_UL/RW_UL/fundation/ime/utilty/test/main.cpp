#include "opencv2/opencv.hpp"

cv::Mat centralCrop(const cv::Mat& img, int cropWidth, int cropHeight) {
	int x = std::max(0, (img.cols - cropWidth) / 2);
	int y = std::max(0, (img.rows - cropHeight) / 2);
	int w = std::min(cropWidth, img.cols - x);
	int h = std::min(cropHeight, img.rows - y);
	return img(cv::Rect(x, y, w, h)).clone();
}

int main()
{
	auto img = cv::imread(R"(C:\Users\rw\Desktop\123.png)");
	if (img.empty()) return -1;

	// 中心裁剪为640x640
	auto cropped = centralCrop(img, 230, 230);

	// 归一化和通道变换
	auto infer_image = cv::dnn::blobFromImage(cropped, 1.f / 255.f, cv::Size(640, 640), cv::Scalar(0, 0, 0), true);

	// 展示裁剪后的图片
	cv::imshow("cropped", cropped);
	cv::waitKey(0);

	return 0;
}