#pragma once

#include"pch.h"

#include"oso_StorageContext.hpp"

using namespace rw::oso;

class StorageContextTest
	:public ::testing::TestWithParam<std::tuple<StorageType>> {
	void SetUp() override {
		_sampleAssembly.setName("OcrDataXmlStoreFormatSampleData");

		ObjectStoreItem boolItem;
		boolItem.setName("bool");
		boolItem.setValueFromBool(false);
		_sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(boolItem));

		ObjectStoreItem doubleItem;
		doubleItem.setName("double");
		doubleItem.setValueFromDouble(1.1);
		_sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(doubleItem));

		ObjectStoreItem intItem;
		intItem.setName("int");
		intItem.setValueFromInt(1);
		_sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(intItem));

		ObjectStoreItem floatItem;
		floatItem.setName("float");
		floatItem.setValueFromFloat(1.1);
		_sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(floatItem));

		ObjectStoreAssembly childAssembly;
		childAssembly.setName("childAssembly");
		ObjectStoreItem longItem;
		longItem.setName("long");
		longItem.setValueFromLong(1);
		childAssembly.addItem(std::make_shared<ObjectStoreItem>(longItem));

		ObjectStoreItem stringItem;
		stringItem.setName("string");
		stringItem.setValueFromString("string");
		childAssembly.addItem(std::make_shared<ObjectStoreItem>(stringItem));

		_sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));
		_sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));
		_sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(boolItem));
		_sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));
		_sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(_sampleAssembly));
	}

	void TearDown() override {
		if (std::filesystem::exists(_testFileName)) {
			std::filesystem::remove(_testFileName);
		}
	}

protected:
	std::shared_ptr<StorageContext>  _testObj;
	std::filesystem::path _testFileName;
	ObjectStoreAssembly _sampleAssembly;
public:
	void initestObj(StorageType type) {
		_testObj = std::make_shared<StorageContext>(type);

		switch (type)
		{
		case StorageType::Xml:
			_testFileName = std::filesystem::current_path() / "test.xml";
			break;
		case StorageType::Json:
			_testFileName = std::filesystem::current_path() / "test.json";
			break;
		case StorageType::Text:
			_testFileName = std::filesystem::current_path() / "test.txt";
			break;
		case StorageType::Sqlite:
			_testFileName = std::filesystem::current_path() / "test.db";
			break;
		default:
			throw std::runtime_error("Unknown type");
		}
	}
};

INSTANTIATE_TEST_SUITE_P(
	StorageContextTests,
	StorageContextTest,
	::testing::Values(
		std::make_tuple(StorageType::Xml),
		std::make_tuple(StorageType::Json)
		//,
		//std::make_tuple(StorageType::Text)
	)
);