#pragma once

#include<string>
#include<vector>

#include"opencv2/opencv.hpp"

namespace rw {
	/**
	* @brief Image pretreatment policy for the model engine.
	*
	* For example, if the model engine requires a specific input size,
	* you can choose to resize the image to that size, maintain the aspect ratio
	*
	* @example
	* For yolov11, the policy is letterbox usually.
	*/
	enum class ImagePretreatmentPolicy
	{
		//Resize the image to the target size, ignoring the aspect ratio.
		Resize = 0,
		//Resize the image to the target size, maintaining the aspect ratio and adding padding.
		//Please set the padding color in the config.
		LetterBox = 1,
		//Resize the image to the target size, maintaining the aspect ratio and cropping the center.
		CenterCrop = 2
	};

	/**
	* @brief Model engine configuration.
	*
	* Some config only works for specific model engine or runtime.
	*/
	struct ModelEngineConfig {
	public:
		//Confidence threshold to filter out low-confidence candidates.
		float conf_threshold = 0.3f;
		//Control the maximum number of candidates to be retained.
		float nms_threshold = 0.4f;
	public:
		//The set of classid will be together for nms
		std::vector<size_t> classids_nms_together{};
	public:
		//The path of the model engine file.
		std::string modelPath;
	public:
		//The type of model engine.
		ImagePretreatmentPolicy imagePretreatmentPolicy = ImagePretreatmentPolicy::LetterBox;
		//The color of the letterbox padding.
		cv::Scalar letterBoxColor{ 0, 0, 0 };
		//The color of the center crop padding.
		cv::Scalar centerCropColor{ 0, 0, 0 };
	};

	enum class ModelType
	{
		//Yolov11 Detection model
		Yolov11_Det,
		//Yolov11 Detection model with CUDA acceleration
		Yolov11_Det_CudaAcc,
		//Yolov11 Segmentation model
		Yolov11_Seg,
		//Yolov11 Segmentation model with CUDA acceleration
		Yolov11_Seg_CudaAcc,
		//Yolov11 Oriented Bounding Box model
		Yolov11_Obb,
		Yolov11_Seg_Mask,
		Yolov11_Seg_Mask_CudaAcc
	};

	inline const char* to_string(ModelType e)
	{
		switch (e)
		{
		case ModelType::Yolov11_Det: return "Yolov11_Det";
		case ModelType::Yolov11_Seg: return "Yolov11_Seg";
		case ModelType::Yolov11_Obb: return "Yolov11_Obb";
		default: return "unknown";
		}
	}

	enum class ModelEngineDeployType
	{
		TensorRT,
		OnnxRuntime
	};

	inline const char* to_string(ModelEngineDeployType e)
	{
		switch (e)
		{
		case ModelEngineDeployType::TensorRT: return "TensorRT";
		case ModelEngineDeployType::OnnxRuntime: return "OnnxRuntime";
		default: return "unknown";
		}
	}
}