#include <numeric>

#include"opencv2/opencv.hpp"

#include"NvInfer.h"
#include"imet_ModelEngine_yolov11_det.hpp"
#include<string>

#include <filesystem>

#include "imet_ModelEngine_yolov11_det_cudaAcc.hpp"
#include "imet_ModelEngine_yolov11_seg_cudaAcc.hpp"
#include "imet_ModelEngine_yolov11_seg_mask_cudaAcc.hpp"

namespace fs = std::filesystem;
using namespace std;
using namespace cv;

class Logger : public nvinfer1::ILogger {
	void log(Severity severity, const char* msg) noexcept override {
	}
}logger;

int main() {
    rw::ModelEngineConfig config;
    config.modelPath = R"(C:\Users\zfkj4090\Desktop\temp\20260101083334\customOO1.engine)";
    rw::imet::ModelEngine_yolov11_det_cudaAcc modelEngine1(config, logger);

	rw::imet::ModelEngine_Yolov11_det modelEngine(R"(C:\Users\zfkj4090\Desktop\temp\20260101083334\customOO1.engine)", logger);





    cv::Mat mat = cv::imread(R"(C:\Users\zfkj4090\Desktop\temp\20260101083334\Image\work1\C\20260101082334744.png)");
    auto result = modelEngine.processImg(mat);
    auto img=modelEngine.draw(mat, result);
    cv::imshow("testImg", img);
    cv::waitKey(0);

  //  // 读取图片文件夹下所有图片路径
  //  std::vector<std::string> imagePaths;
  //  std::string folder = R"(C:\Users\rw\Desktop\temp\images\train)";
  //  for (const auto& entry : fs::directory_iterator(folder)) {
  //      if (entry.is_regular_file()) {
  //          imagePaths.push_back(entry.path().string());
  //      }
  //  }

  //  std::vector<double> times;
  //  int count{ 0 };
  //  for (const auto& imgPath : imagePaths) {
  //      count++;
  //      auto mat = cv::imread(imgPath);
  //      if (mat.empty()) continue;

  //      auto start = std::chrono::high_resolution_clock::now();
  //      auto result = modelEngine.processImg(mat);
  //      auto img = modelEngine.draw(mat, result);
  //      auto end = std::chrono::high_resolution_clock::now();

  //      std::chrono::duration<double, std::milli> elapsed = end - start;
  //      times.push_back(elapsed.count());
		//std::cout << count << std::endl;
  //  }

  //  if (!times.empty()) {
  //      double sum = std::accumulate(times.begin(), times.end(), 0.0);
  //      double avg = sum / times.size();
  //      double minTime = *std::min_element(times.begin(), times.end());
  //      double maxTime = *std::max_element(times.begin(), times.end());
  //      std::cout << "Total images: " << times.size() << std::endl;
  //      std::cout << "Average processImg time: " << avg << " ms" << std::endl;
  //      std::cout << "Min processImg time: " << minTime << " ms" << std::endl;
  //      std::cout << "Max processImg time: " << maxTime << " ms" << std::endl;
  //  }
  //  else {
  //      std::cout << "No valid images found." << std::endl;
  //  }

	///*rw::ModelEngineConfig config;
	//config.modelPath = R"(C:\Users\rw\Desktop\models\niukou_det.engine)";
	//rw::imet::ModelEngine_yolov11_det_refactor_v1 modelEngine(config.modelPath, logger);
	//cv::Mat mat = cv::imread(R"(C:\Users\rw\Desktop\temp\niukou.png)");
	//for (int i=0;i<10;i++)
	//{
	//	auto result = modelEngine.processImg(mat);
	//	auto a=modelEngine.draw(mat, result);
	//	cv::imshow("a", a);
	//	cv::waitKey(0);
	//}*/


    return 0;
}