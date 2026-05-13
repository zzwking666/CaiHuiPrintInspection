#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <windows.h>

namespace rw
{
	namespace actCry
	{
		// 简单的 Windows 注册表管理器
		// 默认根键为 HKEY_CURRENT_USER，可通过构造函数指定其它根键（如 HKEY_LOCAL_MACHINE）
		class RegistryManager
		{
		public:
			explicit RegistryManager(HKEY root = HKEY_CURRENT_USER) noexcept;
			~RegistryManager() = default;

			// 字符串操作（宽字符串）
			bool SetString(const std::wstring& subKey, const std::wstring& valueName, const std::wstring& data) const;
			bool GetString(const std::wstring& subKey, const std::wstring& valueName, std::wstring& outData) const;

			// DWORD（32-bit 无符号整型）
			bool SetDword(const std::wstring& subKey, const std::wstring& valueName, uint32_t value) const;
			bool GetDword(const std::wstring& subKey, const std::wstring& valueName, uint32_t& outValue) const;

			// 删除值或键
			bool DeleteValue(const std::wstring& subKey, const std::wstring& valueName) const;
			bool DeleteKey(const std::wstring& subKey) const;

			// 存在性检查
			bool KeyExists(const std::wstring& subKey) const;
			bool ValueExists(const std::wstring& subKey, const std::wstring& valueName) const;

			// UTF-8 overloads（方便与大多数项目的 std::string 交互）
			bool SetStringUtf8(const std::string& subKey, const std::string& valueName, const std::string& data) const;
			bool GetStringUtf8(const std::string& subKey, const std::string& valueName, std::string& outData) const;

		private:
			HKEY m_root;

			// 内部辅助
			bool OpenKey(const std::wstring& subKey, HKEY& outKey, REGSAM access, bool createIfMissing = false) const;
			static std::wstring Utf8ToWstring(const std::string& utf8);
			static std::string WstringToUtf8(const std::wstring& w);
		};

		std::wstring utf8_to_wstring(const std::string& s);
		

		std::string wstring_to_utf8(const std::wstring& ws);
		
	}
}