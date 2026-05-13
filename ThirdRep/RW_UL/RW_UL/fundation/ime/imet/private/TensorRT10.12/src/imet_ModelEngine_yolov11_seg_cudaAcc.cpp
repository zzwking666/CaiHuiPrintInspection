#include"imet_ModelEngine_yolov11_seg_cudaAcc.hpp"

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
		ModelEngine_yolov11_seg_cudaAcc::ModelEngine_yolov11_seg_cudaAcc(const std::string& modelPath,
			nvinfer1::ILogger& logger)
		{
			init(modelPath, logger);
		}

		ModelEngine_yolov11_seg_cudaAcc::ModelEngine_yolov11_seg_cudaAcc(const ModelEngineConfig& modelCfg,
			nvinfer1::ILogger& logger)
		{
			_config = modelCfg;
			init(modelCfg.modelPath, logger);
		}

		ModelEngine_yolov11_seg_cudaAcc::~ModelEngine_yolov11_seg_cudaAcc()
		{
			destroy_engineRuntime();
			destroy_buffer();
			destroy_cfg();
		}

		void ModelEngine_yolov11_seg_cudaAcc::init(const std::string& enginePath, nvinfer1::ILogger& logger)
		{
			init_engineRuntime(enginePath, logger);
			init_shapeInfo();
			init_buffer();
			ini_cfg();
			warm_up();
		}

		void ModelEngine_yolov11_seg_cudaAcc::init_engineRuntime(const std::string& enginePath, nvinfer1::ILogger& logger)
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

		void ModelEngine_yolov11_seg_cudaAcc::destroy_engineRuntime()
		{
			delete _context;
			delete _engine;
			delete _runtime;
			cudaStreamDestroy(_stream);
		}

		void ModelEngine_yolov11_seg_cudaAcc::init_shapeInfo()
		{
			_inputShape = _engine->getTensorShape(_engine->getIOTensorName(InputShapeIndexForYolov11));
			_outputShape1 = _engine->getTensorShape(_engine->getIOTensorName(OutputShapeIndexForYolov11));
			_outputShape2 = _engine->getTensorShape(_engine->getIOTensorName(OutputShapeIndexForYolov11 + 1));
			_classNum = _outputShape1.d[1] - 4-MaskCoefficientNum;
			_detectionsNum = _outputShape1.d[2];
			_inputHeight = _inputShape.d[2];
			_inputWidth = _inputShape.d[3];
			_channelsNum = _inputShape.d[1];
			_detRows = _outputShape1.d[1] - MaskCoefficientNum;
			_detOutPutSize = _detectionsNum * _detRows;
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
			_outputSize2 = outPut2;
		}

		void ModelEngine_yolov11_seg_cudaAcc::init_buffer()
		{
			cudaMalloc(reinterpret_cast<void**>(&_deviceInputBuffer), _inputSize * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceOutputBuffer1), _outputSize1 * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceOutputBuffer2), _outputSize2 * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceDetSubmatrixBuffer), _detOutPutSize * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceTransposeBuffer), _detOutPutSize * sizeof(float));
			cudaMalloc(reinterpret_cast<void**>(&_deviceDecodeBuffer), (1 + kMaxNumOutputBbox * kNumBoxElement) * sizeof(float));
			_context->setInputTensorAddress(_engine->getIOTensorName(InputShapeIndexForYolov11), _deviceInputBuffer);
			_context->setOutputTensorAddress(_engine->getIOTensorName(OutputShapeIndexForYolov11), _deviceOutputBuffer1);
			_context->setOutputTensorAddress(_engine->getIOTensorName(OutputShapeIndexForYolov11 + 1), _deviceOutputBuffer2);
			_hostOutputBuffer = new float[1 + kMaxNumOutputBbox * kNumBoxElement];
		}

		void ModelEngine_yolov11_seg_cudaAcc::destroy_buffer()
		{
			delete[] _hostOutputBuffer;
			cudaFree(_deviceInputBuffer);
			cudaFree(_deviceOutputBuffer1);
			cudaFree(_deviceTransposeBuffer);
			cudaFree(_deviceDecodeBuffer);
			cudaFree(_deviceDetSubmatrixBuffer);
			cudaFree(_deviceOutputBuffer2);
		}

		void ModelEngine_yolov11_seg_cudaAcc::ini_cfg()
		{
			cudaMalloc((void**)&_deviceClassIdNmsTogether, _config.classids_nms_together.size() * sizeof(size_t));
			cudaMemcpy(_deviceClassIdNmsTogether, _config.classids_nms_together.data(), _config.classids_nms_together.size() * sizeof(size_t), cudaMemcpyHostToDevice);
		}

		void ModelEngine_yolov11_seg_cudaAcc::destroy_cfg()
		{
			cudaFree(_deviceClassIdNmsTogether);
		}

		void ModelEngine_yolov11_seg_cudaAcc::warm_up()
		{
			for (int i = 0; i < 20; i++)
			{
				this->infer();
			}
			cudaDeviceSynchronize();
		}

		void ModelEngine_yolov11_seg_cudaAcc::preprocess(const cv::Mat& mat)
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

		void ModelEngine_yolov11_seg_cudaAcc::infer()
		{
			this->_context->enqueueV3(_stream);
			Utility::copy_submatrix(
				_deviceOutputBuffer1,
				_deviceDetSubmatrixBuffer,
				_outputShape1.d[0],
				_outputShape1.d[1],
				_classNum+4,
				_outputShape1.d[2],
				_stream);
			Utility::transpose(_deviceDetSubmatrixBuffer, _deviceTransposeBuffer, _outputShape1.d[1]-MaskCoefficientNum, _outputShape1.d[2], _stream);
			PostProcess::decode_det(
				_deviceTransposeBuffer,
				_deviceDecodeBuffer,
				_detectionsNum,
				_classNum,
				_config.conf_threshold,
				kMaxNumOutputBbox,
				kNumBoxElement,
				_stream);


			Utility::nms_det(_deviceDecodeBuffer, _config.nms_threshold, kMaxNumOutputBbox, kNumBoxElement, _deviceClassIdNmsTogether, _config.classids_nms_together.size(), _stream);
			cudaMemcpyAsync(_hostOutputBuffer, _deviceDecodeBuffer, (1 + kMaxNumOutputBbox * kNumBoxElement) * sizeof(float), cudaMemcpyDeviceToHost, _stream);
			cudaStreamSynchronize(_stream);
		}

		std::vector<DetectionRectangleInfo> ModelEngine_yolov11_seg_cudaAcc::postProcess()
		{
			std::vector<DetectionRectangleInfo> ret;
			std::vector<Detection> vDetections;
			int count = std::min((int)_hostOutputBuffer[0], kMaxNumOutputBbox);
			for (int i = 0; i < count; i++) {
				int pos = 1 + i * kNumBoxElement;
				int keepFlag = (int)_hostOutputBuffer[pos + 6];
				if (keepFlag == 1) {
					Detection det;
					memcpy(det.bbox, &_hostOutputBuffer[pos], 4 * sizeof(float));
					det.conf = _hostOutputBuffer[pos + 4];
					det.classId = (int)_hostOutputBuffer[pos + 5];

					vDetections.emplace_back(det);
				}
			}

			for (size_t j = 0; j < vDetections.size(); j++) {
				ImgPreprocess::scale_bbox(vDetections[j], _letterBoxInfo);
				DetectionRectangleInfo det_r;
				det_r.center_x = (vDetections[j].bbox[0] + vDetections[j].bbox[2]) / 2;
				det_r.center_y = (vDetections[j].bbox[1] + vDetections[j].bbox[3]) / 2;
				det_r.classId = vDetections[j].classId;
				det_r.score = vDetections[j].conf;
				det_r.width = vDetections[j].bbox[2] - vDetections[j].bbox[0];
				det_r.height = vDetections[j].bbox[3] - vDetections[j].bbox[1];
				det_r.leftTop = { static_cast<int>(vDetections[j].bbox[0]), static_cast<int>(vDetections[j].bbox[1]) };
				det_r.rightTop = { static_cast<int>(vDetections[j].bbox[2]), static_cast<int>(vDetections[j].bbox[1]) };
				det_r.leftBottom = { static_cast<int>(vDetections[j].bbox[0]), static_cast<int>(vDetections[j].bbox[3]) };
				det_r.rightBottom = { static_cast<int>(vDetections[j].bbox[2]), static_cast<int>(vDetections[j].bbox[3]) };
				det_r.area = det_r.width * det_r.height;
				ret.emplace_back(det_r);
			}


			return ret;
		}

		cv::Mat ModelEngine_yolov11_seg_cudaAcc::draw(const cv::Mat& mat,
			const std::vector<DetectionRectangleInfo>& infoList)
		{
			cv::Mat result = mat.clone();
			ImagePainter::PainterConfig config;
			for (const auto& item : infoList)
			{
				std::ostringstream oss;
				oss << "classId:" << item.classId << " score:" << std::fixed << std::setprecision(2) << item.score;
				config.text = oss.str();
				ImagePainter::drawShapesOnSourceImg(result, item, config);
			}
			return result;
		}
	}
}
