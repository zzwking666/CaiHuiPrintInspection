#pragma once

#include<map>
#include<string>

namespace rw
{
	namespace cla
	{
		using ConfigName = std::string;
		using ConfigValue = std::string;

		struct StrConfig
		{
		public:
			std::map<ConfigName, ConfigValue> configs;
		public:
			StrConfig() = default;
			explicit StrConfig(const std::string& serialized);
			operator std::string() const;

			~StrConfig() = default;
			StrConfig(const StrConfig& obj);
			StrConfig& operator=(const StrConfig& obj);
			bool operator==(const StrConfig& obj) const;
			bool operator!=(const StrConfig& obj) const;
		public:
			void insert(const ConfigName& name, const ConfigName& value);
			void erase(const ConfigName& name, const ConfigName& value);
		private:
			std::string serialize() const;
		public:
			static std::string serialize(const StrConfig& config);
			static StrConfig deserialize(const std::string& data);
		};


		struct StrEncryption
		{
			static std::string StrConfigToHex(const StrConfig & cfg, const std::string& privKeyPem);
			static StrConfig StrConfigFromHex(const std::string& str, const std::string& privKeyPem,bool & isOK);
		};
	}

}
