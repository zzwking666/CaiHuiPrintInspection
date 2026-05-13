#include"opencv2/opencv.hpp"

#include"imeo_ModelEngine_yolov11_seg.hpp"
#include<string>

using namespace std;
using namespace cv;

int main() {
	rw::imeo::ModelEngine_Yolov11_Seg modelEngine(R"(C:\Users\rw\Desktop\models\niukou.onnx)");
	auto mat = cv::imread(R"(D:\zfkjData\ButtonScanner\ModelStorage\Temp\Image\work1\bad\NG20250417152301729.png)");
	auto result = modelEngine.processImg(mat);
	auto matResult = modelEngine.draw(mat, result);
	cv::imshow("result", matResult);
	cv::waitKey(0);
	return 0;
}