#include "imet_ModelEngine_yolov11_seg_mask_cudaAcc.hpp"

#include"cuda_device_runtime_api.h"

#include<fstream>
#include<memory>
#include <iomanip>
#include <sstream>

#include "imet_PostProcess.cuh"
#include "imet_PreProcess.cuh"

namespace rw
{
	namespace imet
	{
		ModelEngine_yolov11_seg_mask_cudaAcc::ModelEngine_yolov11_seg_mask_cudaAcc(const std::string& modelPath,
			nvinfer1::ILogger& logger)
		{
			init(modelPath, logger);
		}

		ModelEngine_yolov11_seg_mask_cudaAcc::ModelEngine_yolov11_seg_mask_cudaAcc(const ModelEngineConfig& modelCfg,
			nvinfer1::ILogger& logger)
		{
			_config = modelCfg;
			init(modelCfg.modelPath, logger);
		}

		ModelEngine_yolov11_seg_mask_cudaAcc::~ModelEngine_yolov11_seg_mask_cudaAcc()
		{
			destroy_engineRuntime();
			destroy_buffer();
			destroy_cfg();
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::init(const std::string& enginePath, nvinfer1::ILogger& logger)
		{
			init_engineRuntime(enginePath, logger);
			init_shapeInfo();
			init_buffer();
			ini_cfg();
			warm_up();
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::init_engineRuntime(const std::string& enginePath, nvinfer1::ILogger& logger)
		{
			// Load the engine from the file
			std::ifstream engineStream(enginePath, std::ios::binary);
			engineStream.seekg(0, std::ios::end);
			const size_t modelSize = engineStream.tellg();
			engineStream.seekg(0, std::ios::beg);
			std::unique_ptr<char[]> engineData(new char[modelSize]);
			engineStream.read(engineData.get(), modelSize);
			engineStream.close();

			_runtime = nvinfer1::createInferRuntime(logger);
			_engine = _runtime->deserializeCudaEngine(engineData.get(), modelSize);
			_context = _engine->createExecutionContext();
			cudaStreamCreate(&_stream);
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::destroy_engineRuntime()
		{
			delete _context;
			delete _engine;
			delete _runtime;
			cudaStreamDestroy(_stream);
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::init_shapeInfo()
		{
			_inputShape = _engine->getTensorShape(_engine->getIOTensorName(InputShapeIndexForYolov11));
			_outputShape1 = _engine->getTensorShape(_engine->getIOTensorName(OutputShapeIndexForYolov11));
			_outputShape2 = _engine->getTensorShape(_engine->getIOTensorName(OutputShapeIndexForYolov11 + 1));
			_classNum = _outputShape1.d[1] - 4 - MaskCoefficientNum;
			_detectionsNum = _outputShape1.d[2];
			_inputHeight = _inputShape.d[2];
			_inputWidth = _inputShape.d[3];
			_channelsNum = _inputShape.d[1];
			size_t input = 1;
			for (int i = 0; i < _inputShape.nbDims; i++)
			{
				input *= _inputShape.d[i];
			}
			_inputSize = input;

			size_t outPut = 1;
			for (int i = 0; i < _outputShape1.nbDims; i++)
			{
				outPut *= _outputShape1.d[i];
			}
			_outputSize1 = outPut;

			size_t outPut2 = 1;
			for (int i = 0; i < _outputShape2.nbDims; i++)
			{
				outPut2 *= _outputShape2.d[i];
			}
			_outputSize2 = outPut;
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::init_buffer()
		{
			cudaMalloc(reinterpret_cast<void**>(&_deviceInputBuffer), _inputSize * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceOutputBuffer1), _outputSize1 * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceOutputBuffer2), _outputSize2 * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceTransposeBuffer), _outputSize1 * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceDecodeBuffer), (1 + kMaxNumOutputBbox * kNumBoxElement) * sizeof(float));
			_context->setInputTensorAddress(_engine->getIOTensorName(InputShapeIndexForYolov11), _deviceInputBuffer);
			_context->setOutputTensorAddress(_engine->getIOTensorName(OutputShapeIndexForYolov11), _deviceOutputBuffer1);
			_context->setOutputTensorAddress(_engine->getIOTensorName(OutputShapeIndexForYolov11 + 1), _deviceOutputBuffer2);
			_hostOutputBuffer = new float[1 + kMaxNumOutputBbox * kNumBoxElement];
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::destroy_buffer()
		{
			delete[] _hostOutputBuffer;
			cudaFree(_deviceInputBuffer);
			cudaFree(_deviceOutputBuffer1);
			cudaFree(_deviceTransposeBuffer);
			cudaFree(_deviceDecodeBuffer);
			cudaFree(_deviceOutputBuffer2);
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::ini_cfg()
		{
			cudaMalloc((void**)&_deviceClassIdNmsTogether, _config.classids_nms_together.size() * sizeof(size_t));
			cudaMemcpy(_deviceClassIdNmsTogether, _config.classids_nms_together.data(), _config.classids_nms_together.size() * sizeof(size_t), cudaMemcpyHostToDevice);
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::destroy_cfg()
		{
			cudaFree(_deviceClassIdNmsTogether);
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::process_mask(float* protoDevice, nvinfer1::Dims protoOutDims,
		                                                        std::vector<Detection_seg>& vDetections, int kInputH, int kInputW, cudaStream_t stream, int sourceHeight, int
		                                                        souceWidth)
		{
			int protoC = protoOutDims.d[1];  // default 32
			int protoH = protoOutDims.d[2];  // default 160
			int protoW = protoOutDims.d[3];  // default 160

			int n = vDetections.size();  // number of bboxes
			if (n == 0) return;

			// prepare n x 32 length mask coef space on device
			float* maskCoefDevice = nullptr;
			cudaMalloc(&maskCoefDevice, n * protoC * sizeof(float));
			// prepare n x 160 x 160 mask space on device
			float* maskDevice = nullptr;
			cudaMalloc(&maskDevice, n * protoH * protoW * sizeof(float));

			float* bboxDevice = nullptr;  // x1,y1,x2,y2,x1,y1,x2,y2,...x1,y1,x2,y2
			cudaMalloc(&bboxDevice, n * 4 * sizeof(float));

			for (size_t i = 0; i < n; i++) {
				cudaMemcpyAsync(&maskCoefDevice[i * protoC], vDetections[i].mask, protoC * sizeof(float), cudaMemcpyHostToDevice, stream);
				cudaMemcpyAsync(&bboxDevice[i * 4], vDetections[i].det.bbox, 4 * sizeof(float), cudaMemcpyHostToDevice, stream);
			}

			// mask = sigmoid(mask coef x proto)
			PostProcess::matrix_multiply(maskCoefDevice, n, protoC, protoDevice, protoC, protoH * protoW, maskDevice, stream, true);

			// down sample bbox from 640x640 to 160x160
			float heightRatio = (float)protoH / (float)kInputH;  // 160 / 640 = 0.25
			float widthRatio = (float)protoW / (float)kInputW;  // 160 / 640 = 0.25
			PostProcess::downsample_bbox(bboxDevice, n * 4, heightRatio, widthRatio, stream);

			// set 0 where mask out of bbox
			PostProcess::crop_mask(maskDevice, n, protoH, protoW, bboxDevice, stream);

			// scale mask from 160x160 to original resolution
			// 1. cut mask
			auto cols = souceWidth;
			int rows = sourceHeight;
			float r_w = protoW / (cols * 1.0);
			float r_h = protoH / (rows * 1.0);
			float r = std::min(r_w, r_h);
			float pad_h = (protoH - r * rows) / 2;
			float pad_w = (protoW - r * cols) / 2;
			int cutMaskLeft = (int)pad_w;
			int cutMaskTop = (int)pad_h;
			int cutMaskRight = (int)(protoW - pad_w);
			int cutMaskBottom = (int)(protoH - pad_h);
			int cutMaskWidth = cutMaskRight - cutMaskLeft;
			int cutMaskHeight = cutMaskBottom - cutMaskTop;
			float* cutMaskDevice = nullptr;
			cudaMalloc(&cutMaskDevice, n * cutMaskHeight * cutMaskWidth * sizeof(float));
			PostProcess::cut_mask(maskDevice, n, protoH, protoW, cutMaskDevice, cutMaskTop , cutMaskLeft, cutMaskHeight,
			                      cutMaskWidth, stream);

			// 2. bilinear resize mask
			float* scaledMaskDevice = nullptr;
			cudaMalloc(&scaledMaskDevice, n * rows * cols * sizeof(float));
			PostProcess::resize(cutMaskDevice, n, cutMaskHeight, cutMaskWidth, scaledMaskDevice, rows, cols, stream);

			for (size_t i = 0; i < n; i++) {
				vDetections[i].maskMatrix.resize(rows * cols);
				cudaMemcpyAsync(vDetections[i].maskMatrix.data(), &scaledMaskDevice[i * rows * cols], rows * cols * sizeof(float), cudaMemcpyDeviceToHost, stream);
				cudaDeviceSynchronize();
			}

			cudaFree(maskCoefDevice);
			cudaFree(maskDevice);
			cudaFree(bboxDevice);
			cudaFree(cutMaskDevice);
			cudaFree(scaledMaskDevice);
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::warm_up()
		{
			for (int i = 0; i < 20; i++)
			{
				this->infer();
			}
			cudaDeviceSynchronize();
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::preprocess(const cv::Mat& mat)
		{
			unsigned char pad_b = static_cast<unsigned char>(_config.letterBoxColor[0]);
			unsigned char pad_g = static_cast<unsigned char>(_config.letterBoxColor[1]);
			unsigned char pad_r = static_cast<unsigned char>(_config.letterBoxColor[2]);

			LetterBoxConfig cfg;
			cfg.dstDevData = _deviceInputBuffer;
			cfg.dstHeight = _inputHeight;
			cfg.dstWidth = _inputWidth;
			cfg.pad_b = pad_b;
			cfg.pad_g = pad_g;
			cfg.pad_r = pad_r;
			_letterBoxInfo = ImgPreprocess::LetterBox(mat, cfg, _stream);
		}

		void ModelEngine_yolov11_seg_mask_cudaAcc::infer()
		{
			this->_context->enqueueV3(_stream);
			Utility::transpose(
				_deviceOutputBuffer1,
				_deviceTransposeBuffer,
				_outputShape1.d[1],
				_outputShape1.d[2],
				_stream
			);

			PostProcess::decode_seg(
				_deviceTransposeBuffer,
				_deviceDecodeBuffer,
				_detectionsNum,
				_classNum,
				MaskCoefficientNum,
				_config.conf_threshold,
				kMaxNumOutputBbox,
				kNumBoxElement,
				_stream
			);

			Utility::nms_seg(
				_deviceDecodeBuffer, 
				_config.nms_threshold,
				kMaxNumOutputBbox,
				kNumBoxElement, 
				_deviceClassIdNmsTogether,
				_config.classids_nms_together.size(),
				_stream
			);

			cudaMemcpyAsync(
				_hostOutputBuffer, 
				_deviceDecodeBuffer, 
				(1 + kMaxNumOutputBbox * kNumBoxElement) * sizeof(float), 
				cudaMemcpyDeviceToHost, 
				_stream
			);

			cudaStreamSynchronize(_stream);
		}

		std::vector<DetectionRectangleInfo> ModelEngine_yolov11_seg_mask_cudaAcc::postProcess()
		{
			std::vector<DetectionRectangleInfo> ret;
			std::vector<Detection_seg> vDetections;
			int count = std::min((int)_hostOutputBuffer[0], kMaxNumOutputBbox);
			for (int i = 0; i < count; i++) {
				int pos = 1 + i * kNumBoxElement;
				int keepFlag = (int)_hostOutputBuffer[pos + 6];
				if (keepFlag == 1) {
					Detection_seg det;
					memcpy(det.det.bbox, &_hostOutputBuffer[pos], 4 * sizeof(float));
					det.det.conf = _hostOutputBuffer[pos + 4];
					det.det.classId = (int)_hostOutputBuffer[pos + 5];
					memcpy(det.mask, &_hostOutputBuffer[pos + 7], 32 * sizeof(float));
					vDetections.emplace_back(det);
				}
			}
			process_mask(_deviceOutputBuffer2, _outputShape2, vDetections, _inputHeight, _inputWidth, _stream, _letterBoxInfo.sourceHeight, _letterBoxInfo.sourceWidth);
			for (size_t j = 0; j < vDetections.size(); j++) {
				ImgPreprocess::scale_bbox(vDetections[j].det, _letterBoxInfo);
				DetectionRectangleInfo det_r;
				det_r.center_x = (vDetections[j].det.bbox[0] + vDetections[j].det.bbox[2]) / 2;
				det_r.center_y = (vDetections[j].det.bbox[1] + vDetections[j].det.bbox[3]) / 2;
				det_r.classId = vDetections[j].det.classId;
				det_r.score = vDetections[j].det.conf;
				det_r.width = vDetections[j].det.bbox[2] - vDetections[j].det.bbox[0];
				det_r.height = vDetections[j].det.bbox[3] - vDetections[j].det.bbox[1];
				det_r.leftTop = { static_cast<int>(vDetections[j].det.bbox[0]), static_cast<int>(vDetections[j].det.bbox[1]) };
				det_r.rightTop = { static_cast<int>(vDetections[j].det.bbox[2]), static_cast<int>(vDetections[j].det.bbox[1]) };
				det_r.leftBottom = { static_cast<int>(vDetections[j].det.bbox[0]), static_cast<int>(vDetections[j].det.bbox[3]) };
				det_r.rightBottom = { static_cast<int>(vDetections[j].det.bbox[2]), static_cast<int>(vDetections[j].det.bbox[3]) };
				det_r.segMaskValid = true;


				/*det_r.mask_roi = cv::Mat(_letterBoxInfo.sourceHeight, _letterBoxInfo.sourceWidth, CV_32FC1, vDetections[j].maskMatrix.data());
				int x = std::max(0, static_cast<int>(vDetections[j].det.bbox[0]));
				int y = std::max(0, static_cast<int>(vDetections[j].det.bbox[1]));
				int w = std::min(static_cast<int>(det_r.width), _letterBoxInfo.sourceWidth - x);
				int h = std::min(static_cast<int>(det_r.height), _letterBoxInfo.sourceHeight - y);
				det_r.roi = cv::Rect(x, y, w, h);*/


				cv::Mat full_mask(_letterBoxInfo.sourceHeight, _letterBoxInfo.sourceWidth, CV_32FC1, vDetections[j].maskMatrix.data());

	
				int x = std::max(0, static_cast<int>(vDetections[j].det.bbox[0]));
				int y = std::max(0, static_cast<int>(vDetections[j].det.bbox[1]));
				int w = std::min(static_cast<int>(det_r.width), _letterBoxInfo.sourceWidth - x);
				int h = std::min(static_cast<int>(det_r.height), _letterBoxInfo.sourceHeight - y);
				det_r.roi = cv::Rect(x, y, w, h);


				if (w > 0 && h > 0 && x + w <= full_mask.cols && y + h <= full_mask.rows) {
					det_r.mask_roi = full_mask(det_r.roi).clone();
				}
				else {
					det_r.mask_roi = cv::Mat(); 
				}

				det_r.area = cv::countNonZero(det_r.mask_roi);
				ret.emplace_back(det_r);
			}

			return ret;
		}

		cv::Mat ModelEngine_yolov11_seg_mask_cudaAcc::draw(const cv::Mat& mat,
			const std::vector<DetectionRectangleInfo>& infoList)
		{
			cv::Mat result = mat.clone();
			ImagePainter::PainterConfig config;
			for (const auto& item : infoList)
			{
				std::ostringstream oss;
				oss << "classId:" << item.classId << " score:" << std::fixed << std::setprecision(2) << item.score;
				config.text = oss.str();
				ImagePainter::drawMaskOnSourceImg(result, item, config);
			}

			//// draw mask
			//for (size_t i = 0; i < infoList.size(); i++) {
			//	float* masks = (float*)infoList[i].mask_roi.data;
			//	Utility::draw_mask(result, masks);
			//}
			return result;
		}
	}
}