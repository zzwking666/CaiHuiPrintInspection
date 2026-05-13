#include"oso_StorageStrategy_xml_t.hpp"

namespace oso_StorageStraegy_xml
{
	TEST_F(StorageStrategy_xmlTest, checkStringFormat_getStringAssembly) {
		auto str = rw::oso::StorageStrategy_Xml::getString(std::make_shared<ObjectStoreAssembly>(_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData));
		str = removeNewlinesAndTabs(str);
		auto standardString = _assemblyStandardString;

		EXPECT_EQ(str, standardString);
	}

	TEST_F(StorageStrategy_xmlTest, getStoreAssemblyFromString) {
		auto target = rw::oso::StorageStrategy_Xml::getStoreAssemblyFromString(_assemblyStandardString);

		auto result = (*target) == ObjectStoreAssembly(_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData);
		EXPECT_EQ(result, true);
	}

	TEST_F(StorageStrategy_xmlTest, getStoreItemFromString) {
		auto target = rw::oso::StorageStrategy_Xml::getStoreItemFromString(_itemStandardString);

		auto result = (*target) == ObjectStoreItem(_OsoOrganizeStructureDataXmlStoreFormatItemSampleData);
		EXPECT_EQ(result, true);
	}

	TEST_F(StorageStrategy_xmlTest, xmlCore_getStringAssembly) {
		auto str = testObj.getString(std::make_shared<ObjectStoreAssembly>(_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData));
		str = removeNewlinesAndTabs(str);
		auto standardString = _assemblyStandardString;

		EXPECT_EQ(str, standardString);
	}

	TEST_F(StorageStrategy_xmlTest, xmlCore_getStoreAssemblyFromString) {
		auto target = rw::oso::StorageStrategy_Xml::getStoreAssemblyFromString(_assemblyStandardString);

		auto result = (*target) == ObjectStoreAssembly(_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData);
		EXPECT_EQ(result, true);
	}

	TEST_F(StorageStrategy_xmlTest, xmlCore_getStoreItemFromString) {
		auto target = rw::oso::StorageStrategy_Xml::getStoreItemFromString(_itemStandardString);

		auto result = (*target) == ObjectStoreItem(_OsoOrganizeStructureDataXmlStoreFormatItemSampleData);
		EXPECT_EQ(result, true);
	}

	TEST_F(StorageStrategy_xmlTest, apiSave) {
		EXPECT_TRUE(testObj.save(_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData, "./apiSave.xml"));
	}

	TEST_F(StorageStrategy_xmlTest, apiLoad) {
		EXPECT_TRUE(testObj.save(_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData, "./apiSave.xml"));
		auto result = testObj.load("./apiSave.xml");
		ASSERT_TRUE(result != nullptr);
		auto isEqual = (*result) == _OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData;
		EXPECT_TRUE(isEqual);
	}

	TEST_F(StorageStrategy_xmlTest, apiGetFormatString) {
		auto str = testObj.getFormatString(_OsoOrganizeStructureDataXmlStoreFormatAssemblySampleData);
		auto standardString = _assemblyStandardString;
		standardString = removeNewlinesAndTabs(standardString);
		str = removeNewlinesAndTabs(str);
		EXPECT_EQ(str, standardString);
	}
}