#include"opencv2/opencv.hpp"

#include"NvInfer.h"
#include"ime_ModelEngineFactory.h"
#include<string>

using namespace std;
using namespace cv;

int main() {
	rw::ModelEngineConfig config;
	config.conf_threshold = 0.2f;
	config.nms_threshold = 0.1f;
	config.modelPath = R"(C:\Users\rw\Desktop\models\niukou_det.engine)";
	config.classids_nms_together = { 0,1 };
	config.imagePretreatmentPolicy = rw::ImagePretreatmentPolicy::LetterBox;
	auto model_engine = rw::ModelEngineFactory::createModelEngine(config, rw::ModelType::Yolov11_Det_CudaAcc, rw::ModelEngineDeployType::TensorRT);

	const string path{ R"(C:\Users\rw\Desktop\temp\niukou.png)" };

	Mat image = imread(path);
	if (image.empty())
	{
		cerr << "error reading image: " << path << endl;
	}
	auto start = std::chrono::system_clock::now();
	auto result = model_engine->processImg(image);
	auto resultImage = model_engine->draw(image, result);
	auto end = std::chrono::system_clock::now();

	auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.;
	printf("cost %2.4lf ms\n", tc);

	cv::namedWindow("result", cv::WINDOW_NORMAL);
	cv::resizeWindow("result", 1200, 1080); // 你可以自定义窗口大小
	imshow("result", resultImage);

	cv::waitKey(0);
	return 0;
}