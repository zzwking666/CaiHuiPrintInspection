#include <gtest/gtest.h>
#include "rqw_HistoricalElementManager_t.hpp"

using namespace rw::rqw;

namespace rqw_HistoricalElementManager
{
	TEST(HistoricalElementManagerTest, InsertAndRetrieveElement) {
		using KeyType = int;
		using ImageType = QImage;
		HistoricalElementManager<KeyType, ImageType> manager(10);

		KeyType key = 1;
		QImage image(100, 100, QImage::Format_RGB32);
		ElementInfo<ImageType> elementInfo(image);
		elementInfo.attribute["Author"] = 1;

		manager.insertElement(key, elementInfo);

		auto retrievedElement = manager.getElement(key);
		ASSERT_TRUE(retrievedElement.has_value());
		EXPECT_EQ(retrievedElement->element.size(), image.size());
		EXPECT_EQ(retrievedElement->attribute["Author"], 1);
	}

	TEST(HistoricalElementManagerTest, OverwriteElement) {
		using KeyType = int;
		using ImageType = QImage;
		HistoricalElementManager<KeyType, ImageType> manager(10);

		KeyType key = 2;
		QImage image1(100, 100, QImage::Format_RGB32);
		QImage image2(200, 200, QImage::Format_RGB32);

		ElementInfo<ImageType> elementInfo1(image1);
		ElementInfo<ImageType> elementInfo2(image2);

		manager.insertElement(key, elementInfo1);

		manager.setElement(key, elementInfo2);

		auto retrievedElement = manager.getElement(key);
		ASSERT_TRUE(retrievedElement.has_value());
		EXPECT_EQ(retrievedElement->element.size(), image2.size());
	}

	TEST(HistoricalElementManagerTest, RetrieveNonExistentElement) {
		using KeyType = int;
		using ImageType = QImage;
		HistoricalElementManager<KeyType, ImageType> manager(10);

		KeyType key = 999;

		// 检索不存在的元素
		auto retrievedElement = manager.getElement(key);
		EXPECT_FALSE(retrievedElement.has_value());
	}

	TEST(HistoricalElementManagerTest, insert) {
		using KeyType = int;
		using value = int;
		HistoricalElementManager<KeyType, value> manager(50);
		ElementInfo<value> elementInfo(1);
		for (int i = 0; i < 30; i++)
		{
			manager.insertElement(i, elementInfo);
		}

		ASSERT_EQ(manager.size(), 30);

		for (int i = 30; i < 60; i++)
		{
			manager.insertElement(i, elementInfo);
		}

		ASSERT_EQ(manager.size(), 50);
	}

	TEST(HistoricalElementManagerTest, insertTime) {
		using KeyType = std::chrono::system_clock::time_point; // 使用标准库时间类型
		using value = QImage;
		HistoricalElementManager<KeyType, value> manager(50);
		QImage image(100, 100, QImage::Format_RGB32);
		ElementInfo<value> elementInfo(image);

		for (int i = 0; i < 30; i++) {
			KeyType key = std::chrono::system_clock::now(); // 模拟不同时间点
			manager.insertElement(key, elementInfo);
		}

		ASSERT_EQ(manager.size(), 30);

		for (int i = 30; i < 60; i++) {
			KeyType key = std::chrono::system_clock::now(); // 模拟不同时间点
			manager.insertElement(key, elementInfo);
		}

		ASSERT_EQ(manager.size(), 50);
	}
}