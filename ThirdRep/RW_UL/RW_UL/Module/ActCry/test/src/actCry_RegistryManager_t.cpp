#include"actCry_RegistryManager_t.hpp"

#include "actCry_RegistryManager.hpp"

namespace actCry_RegistryManager
{
	TEST(RegistryManagerClass,api)
	{
		rw::actCry::RegistryManager regMgr;
		const std::wstring subKey = L"Software\\RW\\ActCry\\UnitTest";
		const std::wstring valueNameStr = L"TestString";
		const std::wstring testString = L"Hello, Registry!";
		const std::wstring valueNameDword = L"TestDword";
		const uint32_t testDword = 123456;

		// 测试字符串写入与读取
		EXPECT_TRUE(regMgr.SetString(subKey, valueNameStr, testString));
		std::wstring readString;
		EXPECT_TRUE(regMgr.GetString(subKey, valueNameStr, readString));
		EXPECT_EQ(readString, testString);
		// 测试DWORD写入与读取
		EXPECT_TRUE(regMgr.SetDword(subKey, valueNameDword, testDword));
		uint32_t readDword = 0;
		EXPECT_TRUE(regMgr.GetDword(subKey, valueNameDword, readDword));
		EXPECT_EQ(readDword, testDword);
		// 测试存在性检查
		EXPECT_TRUE(regMgr.KeyExists(subKey));
		EXPECT_TRUE(regMgr.ValueExists(subKey, valueNameStr));
		EXPECT_TRUE(regMgr.ValueExists(subKey, valueNameDword));
		// 测试删除值
		EXPECT_TRUE(regMgr.DeleteValue(subKey, valueNameStr));
		EXPECT_FALSE(regMgr.ValueExists(subKey, valueNameStr));
		EXPECT_TRUE(regMgr.DeleteValue(subKey, valueNameDword));
		EXPECT_FALSE(regMgr.ValueExists(subKey, valueNameDword));
		// 测试删除键
		EXPECT_TRUE(regMgr.DeleteKey(subKey));
		EXPECT_FALSE(regMgr.KeyExists(subKey));

	}
}
