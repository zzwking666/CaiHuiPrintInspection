#pragma once

#include<QImage>
#include <QDateTime>

#include"dsl_CacheFIFO.hpp"
#include"opencv2/opencv.hpp"

namespace rw
{
	namespace rqw
	{
		template <typename ElementType, typename Name = QString, typename Value = double>
		struct ElementInfo
		{
		public:
			ElementType element;
			QMap<Name, Value> attribute;
		public:
			ElementInfo(const ElementType& img) : element(img) {}
			ElementInfo() = default;
		};

		template <typename Name, typename Value>
		struct ElementInfo<cv::Mat, Name, Value>
		{
		public:
			cv::Mat element;
			QMap<Name, Value> attribute;

		public:
			ElementInfo(const cv::Mat& img) : element(img.clone()) {}
			ElementInfo() = default;

			ElementInfo& operator=(const ElementInfo& other) {
				if (this != &other) {
					element = other.element.clone();
					attribute = other.attribute;
				}
				return *this;
			}

			ElementInfo(const ElementInfo& other) : element(other.element.clone()), attribute(other.attribute) {}
		};

		template <typename KeyType, typename ValueType>
		class HistoricalElementManager
		{
		private:
			rw::dsl::CacheFIFO<KeyType, ElementInfo< ValueType>> elementCache;
		public:
			HistoricalElementManager(size_t capacity = 100)
				: elementCache(capacity)
			{
			}

			inline void insertElement(const KeyType& history, const ElementInfo< ValueType>& imageInfo)
			{
				elementCache.set(history, imageInfo);
			}

			inline std::optional<ElementInfo<ValueType>> getElement(const KeyType& history)
			{
				auto result = elementCache.get(history);

				return result;
			}

			inline void setElement(const KeyType& history, const ElementInfo<ValueType>& imageInfo)
			{
				elementCache.set(history, imageInfo);
			}

			inline size_t size()
			{
				return elementCache.size();
			}
		};
	}
}