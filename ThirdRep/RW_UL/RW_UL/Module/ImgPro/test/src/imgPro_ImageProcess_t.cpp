#include"imgPro_ImageProcess_t.hpp"

#include "rqw_ImgConvert.hpp"
#include "ime_utilty.hpp"


TEST_F(ImageProcessTest, ImageProcess)
{
	int argc = 0;
	char* argv[] = { nullptr };
	QApplication app(argc, argv);

	cv::Mat image = cv::imread(R"(C:\Users\rw\Desktop\temp\1 (1).jpg)");


	for (int i=0;i<50;i++)
	{
		(*imgProcess)(image);
	}
	auto maskImg = imgProcess->getMaskImg(image);

	QLabel label;
	label.setPixmap(QPixmap::fromImage(maskImg));
	label.show();

	app.exec();
}

//TEST_F(ImageProcessTest, ImageProcessMutiThread)
//{
//    int argc = 0;
//    char* argv[] = { nullptr };
//    QApplication app(argc, argv);
//
//    cv::Mat image = cv::imread(R"(C:\Users\rw\Desktop\temp\niukou.png)");
//
//    const int threadCount = 8;
//    std::vector<std::thread> threads;
//
//    for (int i = 0; i < threadCount; ++i) {
//        threads.emplace_back([this, &image]() {
//            (*imgProcess)(image);
//            std::cout << imgProcess->getProcessResult().size() + "\n";
//            });
//    }
//
//    for (auto& t : threads) t.join();
//
//    app.exit();
//}