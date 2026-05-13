#include"ActivationCodeModule/cla_ActivationConfig.hpp"

#include <map>
#include <string>
#include <sstream>

namespace rw
{
	namespace cla
	{
		ActivationConfig::ActivationConfig(const std::string& serialized)
		{
			std::istringstream iss(serialized);
			std::string pair;
			while (std::getline(iss, pair, ';'))
			{
				auto pos = pair.find('=');
				if (pos != std::string::npos)
				{
					std::string key = pair.substr(0, pos);
					std::string value = pair.substr(pos + 1);
					configs[key] = value;
				}
				else
				{
					throw std::invalid_argument("Invalid serialized format: " + pair);
				}
			}
		}

		ActivationConfig::operator std::string() const
		{
			return this->serialize();
		}

		ActivationConfig::ActivationConfig(const ActivationConfig& obj)
		{
			this->configs = obj.configs;
		}

		ActivationConfig& ActivationConfig::operator=(const ActivationConfig& obj)
		{
			if (this != &obj)
			{
				this->configs = obj.configs;
			}
			return *this;
		}

		bool ActivationConfig::operator==(const ActivationConfig& obj) const
		{
			return this->configs == obj.configs;
		}

		bool ActivationConfig::operator!=(const ActivationConfig& obj) const
		{
			return !(*this == obj);
		}

		void ActivationConfig::insert(const ConfigName& name, const ConfigName& value)
		{
			configs.insert(std::make_pair(name, value));
		}

		void ActivationConfig::erase(const ConfigName& name, const ConfigName& value)
		{
			auto it = configs.find(name);
			if (it != configs.end() && it->second == value)
			{
				configs.erase(it);
			}
		}

		std::string ActivationConfig::serialize() const
		{
			std::ostringstream oss;
			for (const auto& [key, value] : configs)
			{
				oss << key << "=" << value << ";";
			}
			return oss.str();
		}

		std::string ActivationConfig::serialize(const ActivationConfig& config)
		{
			return config.serialize();
		}

		ActivationConfig ActivationConfig::deserialize(const std::string& data)
		{
			ActivationConfig config;
			std::istringstream iss(data);
			std::string pair;
			while (std::getline(iss, pair, ';'))
			{
				auto pos = pair.find('=');
				if (pos != std::string::npos)
				{
					std::string key = pair.substr(0, pos);
					std::string value = pair.substr(pos + 1);
					config.configs[key] = value;
				}
			}
			return config;
		}
	}
}