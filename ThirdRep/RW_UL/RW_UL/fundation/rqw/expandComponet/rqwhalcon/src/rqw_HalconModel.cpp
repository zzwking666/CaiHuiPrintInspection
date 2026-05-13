#include"rqw_HalconModel.hpp"

#include <halconcpp/HalconCpp.h>

namespace rw
{
	namespace rqw
	{
		HalconShapeId HalconShapeModel::create(const HalconWidgetImg& img, const HalconWidgetObject& rec)
		{
			return create(&img, &rec);
		}

		HalconShapeId HalconShapeModel::create(const HalconWidgetObject* img, const HalconWidgetObject* rec)
		{
			HalconCpp::HObject  ho_TemplateRegion;

			HalconCpp::HObject grayImage;
			HalconCpp::Rgb1ToGray(*img->value(), &grayImage);

			// 提取矩形区域内的内容作为模板学习区域
			HalconCpp::ReduceDomain(grayImage, *rec->value(), &ho_TemplateRegion);

			// 创建模板
			HalconCpp::HTuple hv_shapeId;
			HalconCpp::CreateShapeModel(ho_TemplateRegion, "auto", -0.39, 0.79, "auto", "auto", "use_polarity", "auto", "auto", &hv_shapeId);
			GlobalHalconData::getInstance()._shapeModelIds.push_back(hv_shapeId);
			return hv_shapeId;
		}

		HalconShapeId HalconShapeModel::create(const HalconWidgetObject& img, const HalconWidgetObject* rec)
		{
			return create(&img, rec);
		}

		HalconShapeId HalconShapeModel::create(const HalconWidgetObject* img, const HalconWidgetObject& rec)
		{
			return create(img, &rec);
		}

		HalconShapeId HalconShapeXLDModel::createXLD(const HalconWidgetObject* img, const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& config)
		{
			bool isCreate{ false };
			return createXLD(img, recs, config, isCreate);
		}

		HalconShapeId HalconShapeXLDModel::createXLD(const HalconWidgetObject* img,
			const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& config, bool& isCreate)
		{
			HalconCpp::HObject  ho_TemplateRegion;
			HalconCpp::HTuple hv_ModelID;

			HalconCpp::HObject grayImage;
			HalconCpp::Rgb1ToGray(*img->value(), &grayImage);

			// 合并所有区域

			// 检查输入是否为空
			if (recs.empty())
			{
				isCreate = false;
				return HalconShapeId();
			}

			// 初始化合并结果为第一个区域
			HalconCpp::HObject mergedRegion = *recs[0].value();

			// 遍历并合并所有区域
			for (size_t i = 1; i < recs.size(); ++i)
			{
				HalconCpp::HObject temp;
				HalconCpp::Union2(mergedRegion, *recs[i].value(), &temp);
				mergedRegion = temp;
			}

			HalconCpp::ReduceDomain(grayImage, mergedRegion, &ho_TemplateRegion);

			// 边缘提取
			HalconCpp::HObject ho_Border;
			HalconCpp::EdgesSubPix(ho_TemplateRegion, &ho_Border, "canny", 3, 5, 10);

			// 选择轮廓长度大于a的边缘
			HalconCpp::HObject ho_SelectedBorder;
			HalconCpp::SelectShapeXld(ho_Border, &ho_SelectedBorder, "contlength", "and", config.minXldLenth, 10000000);

			// 统计轮廓数量
			HalconCpp::HTuple number;
			HalconCpp::CountObj(ho_SelectedBorder, &number);

			if (number.D() > 0)
			{
				// 创建形状模板
				HalconCpp::CreateShapeModelXld(
					ho_SelectedBorder,
					"auto",
					HalconCpp::HTuple(0).TupleRad(),
					HalconCpp::HTuple(360).TupleRad(),
					"auto",
					"auto",
					"use_polarity",
					5,
					&hv_ModelID
				);
				GlobalHalconData::getInstance()._shapeModelIds.push_back(hv_ModelID);
				isCreate = true;
			}
			else
			{
				isCreate = false;
				return HalconShapeId();
			}
			return hv_ModelID;
		}

		std::vector<HalconWidgetTemplateResult> HalconShapeXLDModel::findShapeModel(const HalconShapeId& id,
			const HalconWidgetObject& img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig,
			const PainterConfig& config)
		{
			return findShapeModel(id, &img, halconShapeXldFindConfig, config);
		}

		std::vector<HalconWidgetTemplateResult> HalconShapeXLDModel::findShapeModel(const HalconShapeId& id, const HalconWidgetObject* img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig, const PainterConfig& painterConfig)
		{
			if (!img)
			{
				return std::vector<HalconWidgetTemplateResult>();
			}

			if (!img->has_value())
			{
				return std::vector<HalconWidgetTemplateResult>();
			}

			if (img->type != HalconObjectType::Image)
			{
				return std::vector<HalconWidgetTemplateResult>();
			}

			// 获取图像对象
			HalconCpp::HObject* image = img->value();
			HalconCpp::HObject grayImage;
			HalconCpp::Rgb1ToGray(*image, &grayImage);

			// 执行模板匹配
			HalconCpp::HTuple row, column, angle, score;
			HalconCpp::FindShapeModel(grayImage, id, -0.39, 0.79, halconShapeXldFindConfig.minScore, halconShapeXldFindConfig.findNumber, 0.5, "least_squares", 0, 0.9, &row, &column, &angle, &score);

			// 存储匹配结果
			std::vector<HalconWidgetTemplateResult> results;

			// 检查是否找到匹配
			if (row.TupleLength() > 0)
			{
				// 获取模板轮廓
				HalconCpp::HObject modelContours;
				HalconCpp::GetShapeModelContours(&modelContours, id, 1);

				// 遍历所有匹配结果
				for (int i = 0; i < row.TupleLength(); ++i)
				{
					// 计算仿射变换矩阵
					HalconCpp::HTuple homMat2D;
					HalconCpp::VectorAngleToRigid(0, 0, 0, row[i], column[i], angle[i], &homMat2D);

					// 仿射变换模板轮廓
					HalconCpp::HObject transformedContours;
					HalconCpp::AffineTransContourXld(modelContours, &transformedContours, homMat2D);

					// 创建匹配结果对象
					HalconWidgetTemplateResult result(new HalconCpp::HObject(transformedContours));
					result.score = score[i].D(); // 设置匹配分数
					result.painterConfig = painterConfig;

					// 设置位置信息
					result.row = row[i].D();      // 匹配中心的行坐标
					result.column = column[i].D(); // 匹配中心的列坐标
					result.angle = angle[i].D();   // 匹配的旋转角度
					result.descrption = "XLDFindResult";
					result.type = HalconObjectType::TemplateResult;
					// 添加到结果列表
					results.push_back(std::move(result));
				}
			}

			// 如果没有找到匹配，返回空结果列表
			return results;
		}

		std::vector<HalconWidgetTemplateResult> HalconShapeXLDModel::findShapeModel(const HalconShapeId& id,
			const HalconWidgetObject* img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig)
		{
			return findShapeModel(id, img, halconShapeXldFindConfig, PainterConfig());
		}

		std::vector<HalconWidgetTemplateResult> HalconShapeXLDModel::findShapeModel(const HalconShapeId& id,
			const HalconWidgetObject& img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig)
		{
			return findShapeModel(id, img, halconShapeXldFindConfig, PainterConfig());
		}

		void HalconShapeXLDModel::saveModel(const HalconShapeId& id, const std::string& filePath)
		{
			HalconCpp::WriteShapeModel(id, filePath.c_str());
		}

		HalconShapeId HalconShapeXLDModel::readModel(const std::string& filePath)
		{
			HalconShapeId result;
			HalconCpp::ReadShapeModel(filePath.c_str(), &result);
			return result;
		}

		std::vector<HalconWidgetTemplateResult> HalconShapeModel::findShapeModel(
			const HalconShapeId& modelId, const HalconWidgetObject& img)
		{
			return findShapeModel(modelId, &img);
		}

		std::vector<HalconWidgetTemplateResult> HalconShapeModel::findShapeModel(const HalconShapeId& modelId,
			const HalconWidgetObject* img)
		{
			return findShapeModel(modelId, img, PainterConfig());
		}

		std::vector<HalconWidgetTemplateResult> HalconShapeModel::findShapeModel(const HalconShapeId& modelId,
			const HalconWidgetObject* img, const PainterConfig& config)
		{
			if (!img)
			{
				return std::vector<HalconWidgetTemplateResult>();
			}

			// 检查输入对象是否有效
			if (!img->has_value())
			{
				return std::vector<HalconWidgetTemplateResult>();
			}

			if (img->type != HalconObjectType::Image)
			{
				return std::vector<HalconWidgetTemplateResult>();
			}

			// 获取图像对象
			HalconCpp::HObject* image = img->value();
			HalconCpp::HObject grayImage;
			HalconCpp::Rgb1ToGray(*image, &grayImage);

			// 执行模板匹配
			HalconCpp::HTuple row, column, angle, score;
			HalconCpp::FindShapeModel(grayImage, modelId, -0.39, 0.79, 0.5, 1, 0.5, "least_squares", 0, 0.9, &row, &column, &angle, &score);

			// 存储匹配结果
			std::vector<HalconWidgetTemplateResult> results;

			// 检查是否找到匹配
			if (row.TupleLength() > 0)
			{
				// 获取模板轮廓
				HalconCpp::HObject modelContours;
				HalconCpp::GetShapeModelContours(&modelContours, modelId, 1);

				// 遍历所有匹配结果
				for (int i = 0; i < row.TupleLength(); ++i)
				{
					// 计算仿射变换矩阵
					HalconCpp::HTuple homMat2D;
					HalconCpp::VectorAngleToRigid(0, 0, 0, row[i], column[i], angle[i], &homMat2D);

					// 仿射变换模板轮廓
					HalconCpp::HObject transformedContours;
					HalconCpp::AffineTransContourXld(modelContours, &transformedContours, homMat2D);

					// 创建匹配结果对象
					HalconWidgetTemplateResult result(new HalconCpp::HObject(transformedContours));
					result.score = score[i].D(); // 设置匹配分数
					result.painterConfig = config;

					// 设置位置信息
					result.row = row[i].D();      // 匹配中心的行坐标
					result.column = column[i].D(); // 匹配中心的列坐标
					result.angle = angle[i].D();   // 匹配的旋转角度
					result.type = HalconObjectType::TemplateResult;

					// 添加到结果列表
					results.push_back(std::move(result));
				}
			}

			// 如果没有找到匹配，返回空结果列表
			return results;
		}

		void HalconShapeModel::saveModel(const HalconShapeId& id, const std::string& filePath)
		{
			HalconCpp::WriteShapeModel(id, filePath.c_str());
		}

		HalconShapeId HalconShapeModel::readModel(const std::string& filePath)
		{
			HalconShapeId result;
			HalconCpp::ReadShapeModel(filePath.c_str(), &result);
			return result;
		}

		HalconShapeId HalconShapeXLDModel::createXLD(const HalconWidgetObject& img,
			const std::vector<HalconWidgetObject>& recs)
		{
			bool isCreate{ false };
			return createXLD(&img, recs, HalconShapeXLDConfig(), isCreate);
		}

		HalconShapeId HalconShapeXLDModel::createXLD(const HalconWidgetObject& img,
			const std::vector<HalconWidgetObject>& recs, bool& isCreate)
		{
			return createXLD(&img, recs, HalconShapeXLDConfig(), isCreate);
		}

		HalconShapeId HalconShapeXLDModel::createXLD(const HalconWidgetObject& img,
			const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& config)
		{
			bool isCreate{ false };
			return createXLD(&img, recs, config, isCreate);
		}
	}
}