#pragma once

#include"pch.h"

#include"oso_StorageStrategy_Xml.hpp"

using namespace rw::oso;

class StorageStrategy_xmlTest
	: public ::testing::Test {
protected:
	StorageStrategy_Xml testObj;

	ObjectStoreAssembly _OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData;
	string _assemblyStandardString;

	ObjectStoreItem _OsoOrganizeStructureDataXmlStoreFormatItemSampleData;
	string _itemStandardString;
private:
	void setObjectStoreAssembly() {
		_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData.setName("OcrDataXmlStoreFormatSampleData");

		ObjectStoreItem boolItem;
		boolItem.setName("bool");
		boolItem.setValueFromBool(false);
		_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData.addItem(std::make_shared<ObjectStoreItem>(boolItem));

		ObjectStoreItem doubleItem;
		doubleItem.setName("double");
		doubleItem.setValueFromDouble(1.1);
		_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData.addItem(std::make_shared<ObjectStoreItem>(doubleItem));

		ObjectStoreItem intItem;
		intItem.setName("int");
		intItem.setValueFromInt(1);
		_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData.addItem(std::make_shared<ObjectStoreItem>(intItem));

		ObjectStoreItem floatItem;
		floatItem.setName("float");
		floatItem.setValueFromFloat(1.1);
		_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData.addItem(std::make_shared<ObjectStoreItem>(floatItem));

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

		_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));
	}
	void setObjectStoreItem() {
		_OsoOrganizeStructureDataXmlStoreFormatItemSampleData.setName("OcrDataXmlStoreFormatSampleData");
		_OsoOrganizeStructureDataXmlStoreFormatItemSampleData.setValueFromString("test");
	}
	void setAssemblyStandardString() {
		/*    */_assemblyStandardString.append(R"(<ObjectStoreAssembly name="OcrDataXmlStoreFormatSampleData">)");
		/*        */_assemblyStandardString.append(R"(<children>)");
		/*            */_assemblyStandardString.append(R"(<ObjectStoreItem name="bool">)");
		/*                 */_assemblyStandardString.append(R"(<value>false</value>)");
		/*                 */_assemblyStandardString.append(R"(<type>bool</type>)");
		/*            */_assemblyStandardString.append(R"(</ObjectStoreItem>)");
		/*            */_assemblyStandardString.append(R"(<ObjectStoreItem name="double">)");
		/*                */_assemblyStandardString.append(R"(<value>1.100000</value>)");
		/*                */_assemblyStandardString.append(R"(<type>double</type>)");
		/*            */_assemblyStandardString.append(R"(</ObjectStoreItem>)");
		/*            */_assemblyStandardString.append(R"(<ObjectStoreItem name="int">)");
		/*                */_assemblyStandardString.append(R"(<value>1</value>)");
		/*                */_assemblyStandardString.append(R"(<type>int</type>)");
		/*            */_assemblyStandardString.append(R"(</ObjectStoreItem>)");
		/*            */_assemblyStandardString.append(R"(<ObjectStoreItem name="float">)");
		/*               */_assemblyStandardString.append(R"(<value>1.100000</value>)");
		/*               */_assemblyStandardString.append(R"(<type>float</type>)");
		/*            */_assemblyStandardString.append(R"(</ObjectStoreItem>)");
		/*            */_assemblyStandardString.append(R"(<ObjectStoreAssembly name="childAssembly">)");
		/*              */_assemblyStandardString.append(R"(<children>)");
		/*                   */_assemblyStandardString.append(R"(<ObjectStoreItem name="long">)");
		/*                       */_assemblyStandardString.append(R"(<value>1</value>)");
		/*                       */_assemblyStandardString.append(R"(<type>long</type>)");
		/*                   */_assemblyStandardString.append(R"(</ObjectStoreItem>)");
		/*                   */_assemblyStandardString.append(R"(<ObjectStoreItem name="string">)");
		/*                       */_assemblyStandardString.append(R"(<value>string</value>)");
		/*                       */_assemblyStandardString.append(R"(<type>string</type>)");
		/*                   */_assemblyStandardString.append(R"(</ObjectStoreItem>)");
		/*              */_assemblyStandardString.append(R"(</children>)");
		/*            */_assemblyStandardString.append(R"(</ObjectStoreAssembly>)");
		/**/        _assemblyStandardString.append(R"(</children>)");
		/**/    _assemblyStandardString.append(R"(</ObjectStoreAssembly>)");
	}
	void setItemStandardString() {
		/*    */_itemStandardString.append(R"(<ObjectStoreItem name="OcrDataXmlStoreFormatSampleData">)");
		/*        */_itemStandardString.append(R"(<value>test</value>)");
		/*        */_itemStandardString.append(R"(<type>string</type>)");
		/*    */_itemStandardString.append(R"(</ObjectStoreItem>)");
	}
protected:
	void SetUp() override {
		setObjectStoreAssembly();
		setObjectStoreItem();
		setAssemblyStandardString();
		setItemStandardString();
	}

	void TearDown() override {
	}
public:
	static std::string removeNewlinesAndTabs(const std::string& input) {
		std::string result = input;
		result.erase(std::remove_if(result.begin(), result.end(), [](char c) {
			return c == '\n' || c == '\t';
			}), result.end());
		return result;
	}
};