#pragma once

#include<map>
#include<string>

namespace rw
{
	namespace cla
	{
		using ConfigName = std::string;
		using ConfigValue = std::string;

		struct ActivationConfig
		{
		private:
			std::map<ConfigName, ConfigValue> configs;
		public:
			ActivationConfig() = default;
			explicit ActivationConfig(const std::string& serialized);
			operator std::string() const;

			~ActivationConfig() = default;
			ActivationConfig(const ActivationConfig& obj);
			ActivationConfig& operator=(const ActivationConfig& obj);
			bool operator==(const ActivationConfig& obj) const;
			bool operator!=(const ActivationConfig& obj) const;
		public:
			void insert(const ConfigName & name,const ConfigName &value);
			void erase(const ConfigName& name, const ConfigName& value);
		private:
			std::string serialize() const;
		public:
			static std::string serialize(const ActivationConfig& config);
			static ActivationConfig deserialize(const std::string& data);

		};
	}
}
