#include"oso_StorageContext_t.hpp"

#include <fstream>

namespace oso_StorageContext
{
	TEST_P(StorageContextTest, apiSaveByPath) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->save(_sampleAssembly, _testFileName));
	}

	TEST_P(StorageContextTest, apiSaveByPathString) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->save(_sampleAssembly, _testFileName.c_str()));
	}

	TEST_P(StorageContextTest, apiLoadByPath) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->save(_sampleAssembly, _testFileName));

		auto loadedAssembly = _testObj->load(_testFileName);
		ASSERT_TRUE(loadedAssembly != nullptr);
		ASSERT_EQ(loadedAssembly->getName(), _sampleAssembly.getName());
		ASSERT_EQ(loadedAssembly->getItems().size(), _sampleAssembly.getItems().size());
		ASSERT_EQ((*loadedAssembly.get()), _sampleAssembly);
	}

	TEST_P(StorageContextTest, apiLoadByPathString) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->save(_sampleAssembly, _testFileName.c_str()));

		auto loadedAssembly = _testObj->load(_testFileName);
		ASSERT_TRUE(loadedAssembly != nullptr);
		ASSERT_EQ(loadedAssembly->getName(), _sampleAssembly.getName());
		ASSERT_EQ(loadedAssembly->getItems().size(), _sampleAssembly.getItems().size());
		ASSERT_EQ((*loadedAssembly.get()), _sampleAssembly);
	}

	TEST_P(StorageContextTest, apiGetFormatString) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->save(_sampleAssembly, _testFileName));

		auto loadedAssembly = _testObj->load(_testFileName);
		ASSERT_TRUE(loadedAssembly != nullptr);
		ASSERT_EQ(loadedAssembly->getName(), _sampleAssembly.getName());
		ASSERT_EQ(loadedAssembly->getItems().size(), _sampleAssembly.getItems().size());
		ASSERT_EQ(_testObj->getFormatString(*loadedAssembly), _testObj->getFormatString(_sampleAssembly));
	}

	TEST_P(StorageContextTest, apiSaveSafeByPath) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->saveSafe(_sampleAssembly, _testFileName));
	}

	TEST_P(StorageContextTest, apiLoadSafeByPath) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->saveSafe(_sampleAssembly, _testFileName));

		auto loadedAssembly = _testObj->loadSafe(_testFileName);
		ASSERT_TRUE(loadedAssembly != nullptr);
		ASSERT_EQ(loadedAssembly->getName(), _sampleAssembly.getName());
		ASSERT_EQ(loadedAssembly->getItems().size(), _sampleAssembly.getItems().size());
		ASSERT_EQ((*loadedAssembly.get()), _sampleAssembly);
	}

	TEST_P(StorageContextTest, apiLoadSafeByPathWithResult) {
		auto& [type] = GetParam();
		initestObj(type);
		ASSERT_TRUE(_testObj->saveSafe(_sampleAssembly, _testFileName));

		FileReadResult readResult;
		auto loadedAssembly = _testObj->loadSafe(_testFileName, readResult);
		ASSERT_TRUE(loadedAssembly != nullptr);
		ASSERT_EQ(readResult, FileReadResult::Success);
		ASSERT_EQ(loadedAssembly->getName(), _sampleAssembly.getName());
		ASSERT_EQ(loadedAssembly->getItems().size(), _sampleAssembly.getItems().size());
		ASSERT_EQ((*loadedAssembly.get()), _sampleAssembly);
	}

	TEST_P(StorageContextTest, apiLoadSafeByPathNotFound) {
		auto& [type] = GetParam();
		initestObj(type);

		FileReadResult readResult;
		auto loadedAssembly = _testObj->loadSafe(_testFileName, readResult);
		ASSERT_EQ(loadedAssembly, nullptr);
		ASSERT_EQ(readResult, FileReadResult::NotFound);
	}

	TEST_P(StorageContextTest, apiLoadSafeByPathWithBackupUsed) {
		auto& [type] = GetParam();
		initestObj(type);

		_testObj->ensureFileExists(_testFileName,_sampleAssembly);
		// 1. 正常保存，生成主文件和bak文件
		ASSERT_TRUE(_testObj->saveSafe(_sampleAssembly, _testFileName));

		// 2. 手动破坏主文件（写入无效内容）
		{
			std::ofstream ofs(_testFileName, std::ios::trunc);
			ofs << "invalid xml or data";
		}

		// 3. 尝试用loadSafe读取，应该读取bak文件
		FileReadResult readResult;
		auto loadedAssembly = _testObj->loadSafe(_testFileName, readResult);
		ASSERT_TRUE(loadedAssembly != nullptr);
		ASSERT_EQ(readResult, FileReadResult::BackupUsed);
		ASSERT_EQ(loadedAssembly->getName(), _sampleAssembly.getName());
		ASSERT_EQ(loadedAssembly->getItems().size(), _sampleAssembly.getItems().size());
		ASSERT_EQ((*loadedAssembly.get()), _sampleAssembly);

		std::filesystem::path bakFile = _testFileName;
		bakFile += ".bak";
		if (std::filesystem::exists(bakFile)) {
			std::filesystem::remove(bakFile);
		}
	}
}
