#include "actCry_RegistryManager.hpp"

#include <windows.h>
#include <vector>

namespace rw
{
	namespace actCry
	{
		RegistryManager::RegistryManager(HKEY root) noexcept
			: m_root(root)
		{
		}

		bool RegistryManager::OpenKey(const std::wstring& subKey, HKEY& outKey, REGSAM access, bool createIfMissing) const
		{
			outKey = nullptr;
			LONG result;
			if (createIfMissing)
			{
				DWORD disposition = 0;
				result = RegCreateKeyExW(
					m_root,
					subKey.c_str(),
					0,
					nullptr,
					REG_OPTION_NON_VOLATILE,
					access,
					nullptr,
					&outKey,
					&disposition);
			}
			else
			{
				result = RegOpenKeyExW(m_root, subKey.c_str(), 0, access, &outKey);
			}

			return (result == ERROR_SUCCESS);
		}

		bool RegistryManager::SetString(const std::wstring& subKey, const std::wstring& valueName, const std::wstring& data) const
		{
			HKEY key = nullptr;
			if (!OpenKey(subKey, key, KEY_WRITE, true))
				return false;

			LONG res = RegSetValueExW(
				key,
				valueName.empty() ? nullptr : valueName.c_str(),
				0,
				REG_SZ,
				reinterpret_cast<const BYTE*>(data.c_str()),
				static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t)));

			RegCloseKey(key);
			return (res == ERROR_SUCCESS);
		}

		bool RegistryManager::GetString(const std::wstring& subKey, const std::wstring& valueName, std::wstring& outData) const
		{
			HKEY key = nullptr;
			if (!OpenKey(subKey, key, KEY_READ, false))
				return false;

			DWORD type = 0;
			DWORD size = 0;
			LONG res = RegQueryValueExW(
				key,
				valueName.empty() ? nullptr : valueName.c_str(),
				nullptr,
				&type,
				nullptr,
				&size);

			if (res != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ))
			{
				RegCloseKey(key);
				return false;
			}

			std::vector<wchar_t> buffer(size / sizeof(wchar_t));
			res = RegQueryValueExW(
				key,
				valueName.empty() ? nullptr : valueName.c_str(),
				nullptr,
				&type,
				reinterpret_cast<BYTE*>(buffer.data()),
				&size);

			RegCloseKey(key);

			if (res != ERROR_SUCCESS)
				return false;

			// 确保以 null 结尾
			outData.assign(buffer.data(), buffer.size());
			// trim trailing nulls
			size_t pos = outData.find_last_not_of(L'\0');
			if (pos != std::wstring::npos)
				outData.resize(pos + 1);
			else
				outData.clear();

			return true;
		}

		bool RegistryManager::SetDword(const std::wstring& subKey, const std::wstring& valueName, uint32_t value) const
		{
			HKEY key = nullptr;
			if (!OpenKey(subKey, key, KEY_WRITE, true))
				return false;

			DWORD val = static_cast<DWORD>(value);
			LONG res = RegSetValueExW(
				key,
				valueName.empty() ? nullptr : valueName.c_str(),
				0,
				REG_DWORD,
				reinterpret_cast<const BYTE*>(&val),
				sizeof(val));

			RegCloseKey(key);
			return (res == ERROR_SUCCESS);
		}

		bool RegistryManager::GetDword(const std::wstring& subKey, const std::wstring& valueName, uint32_t& outValue) const
		{
			HKEY key = nullptr;
			if (!OpenKey(subKey, key, KEY_READ, false))
				return false;

			DWORD type = 0;
			DWORD size = sizeof(DWORD);
			DWORD val = 0;
			LONG res = RegQueryValueExW(
				key,
				valueName.empty() ? nullptr : valueName.c_str(),
				nullptr,
				&type,
				reinterpret_cast<BYTE*>(&val),
				&size);

			RegCloseKey(key);

			if (res != ERROR_SUCCESS || type != REG_DWORD)
				return false;

			outValue = static_cast<uint32_t>(val);
			return true;
		}

		bool RegistryManager::DeleteValue(const std::wstring& subKey, const std::wstring& valueName) const
		{
			HKEY key = nullptr;
			if (!OpenKey(subKey, key, KEY_SET_VALUE, false))
				return false;

			LONG res = RegDeleteValueW(key, valueName.empty() ? nullptr : valueName.c_str());
			RegCloseKey(key);
			return (res == ERROR_SUCCESS);
		}

		bool RegistryManager::DeleteKey(const std::wstring& subKey) const
		{
			// 注意：RegDeleteKeyExW 在旧系统上可能不可用，这里使用 RegDeleteKeyW（它在 Windows Vista+ 可用）
			LONG res = RegDeleteKeyW(m_root, subKey.c_str());
			return (res == ERROR_SUCCESS);
		}

		bool RegistryManager::KeyExists(const std::wstring& subKey) const
		{
			HKEY key = nullptr;
			bool ok = OpenKey(subKey, key, KEY_READ, false);
			if (ok)
				RegCloseKey(key);
			return ok;
		}

		bool RegistryManager::ValueExists(const std::wstring& subKey, const std::wstring& valueName) const
		{
			HKEY key = nullptr;
			if (!OpenKey(subKey, key, KEY_READ, false))
				return false;

			LONG res = RegQueryValueExW(key, valueName.empty() ? nullptr : valueName.c_str(), nullptr, nullptr, nullptr, nullptr);
			RegCloseKey(key);
			return (res == ERROR_SUCCESS);
		}

		// UTF-8 辅助
		std::wstring RegistryManager::Utf8ToWstring(const std::string& utf8)
		{
			if (utf8.empty())
				return std::wstring();

			int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), nullptr, 0);
			if (size_needed <= 0)
				return std::wstring();

			std::wstring w(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), &w[0], size_needed);
			return w;
		}

		std::string RegistryManager::WstringToUtf8(const std::wstring& w)
		{
			if (w.empty())
				return std::string();

			int size_needed = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), nullptr, 0, nullptr, nullptr);
			if (size_needed <= 0)
				return std::string();

			std::string s(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), &s[0], size_needed, nullptr, nullptr);
			return s;
		}

		std::wstring utf8_to_wstring(const std::string& s)
		{
			if (s.empty()) return {};
			int len = ::MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
			if (len <= 0) return {};
			std::wstring w;
			w.resize(len);
			::MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), &w[0], len);
			return w;
		}

		std::string wstring_to_utf8(const std::wstring& ws)
		{
			if (ws.empty()) return {};
			int len = ::WideCharToMultiByte(CP_UTF8, 0, ws.data(), static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);
			if (len <= 0) return {};
			std::string s;
			s.resize(len);
			::WideCharToMultiByte(CP_UTF8, 0, ws.data(), static_cast<int>(ws.size()), &s[0], len, nullptr, nullptr);
			return s;
		}

		// UTF-8 overloads
		bool RegistryManager::SetStringUtf8(const std::string& subKey, const std::string& valueName, const std::string& data) const
		{
			return SetString(Utf8ToWstring(subKey), Utf8ToWstring(valueName), Utf8ToWstring(data));
		}

		bool RegistryManager::GetStringUtf8(const std::string& subKey, const std::string& valueName, std::string& outData) const
		{
			std::wstring tmp;
			if (!GetString(Utf8ToWstring(subKey), Utf8ToWstring(valueName), tmp))
				return false;
			outData = WstringToUtf8(tmp);
			return true;
		}
	}
}