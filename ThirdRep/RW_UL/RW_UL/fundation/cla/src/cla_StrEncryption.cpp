#include"cla_StrEncryption.hpp"

#include <map>
#include <string>
#include <sstream>

#include "cla_EncrpyUtilty.hpp"
#include "cla_SymmetricEncryptor.hpp"

namespace rw
{
	namespace cla
	{
		StrConfig::StrConfig(const std::string& serialized)
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

		StrConfig::operator std::string() const
		{
			return this->serialize();
		}

		StrConfig::StrConfig(const StrConfig& obj)
		{
			this->configs = obj.configs;
		}

		StrConfig& StrConfig::operator=(const StrConfig& obj)
		{
			if (this != &obj)
			{
				this->configs = obj.configs;
			}
			return *this;
		}

		bool StrConfig::operator==(const StrConfig& obj) const
		{
			return this->configs == obj.configs;
		}

		bool StrConfig::operator!=(const StrConfig& obj) const
		{
			return !(*this == obj);
		}

		void StrConfig::insert(const ConfigName& name, const ConfigName& value)
		{
			configs.insert(std::make_pair(name, value));
		}

		void StrConfig::erase(const ConfigName& name, const ConfigName& value)
		{
			auto it = configs.find(name);
			if (it != configs.end() && it->second == value)
			{
				configs.erase(it);
			}
		}

		std::string StrConfig::serialize() const
		{
			std::ostringstream oss;
			for (const auto& [key, value] : configs)
			{
				oss << key << "=" << value << ";";
			}
			return oss.str();
		}

		std::string StrConfig::serialize(const StrConfig& config)
		{
			return config.serialize();
		}

		StrConfig StrConfig::deserialize(const std::string& data)
		{
			StrConfig config;
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

		std::string StrEncryption::StrConfigToHex(const StrConfig& cfg, const std::string& privKeyPem)
		{
			auto str = StrConfig::serialize(cfg);

			SymmetricEncryptorContext ctx;
			ctx.bits = ActivationBits::Hexadecimal;
			ctx.sourceString = str;
			ctx.key = KeyGenerator::generate32BitKey(privKeyPem);

			SymmetricEncryptor encryptor(ctx);
			encryptor.encrypt();

			return encryptor.context().encryptString;
		}

		StrConfig StrEncryption::StrConfigFromHex(const std::string& str, const std::string& privKeyPem, bool& isOK)
		{
			try
			{
				SymmetricEncryptorContext ctx;
				ctx.bits = ActivationBits::Hexadecimal;
				ctx.encryptString = str;
				ctx.key = KeyGenerator::generate32BitKey(privKeyPem);

				SymmetricEncryptor decryptor(ctx);
				decryptor.decrypt();

				const std::string& plain = decryptor.context().sourceString;

				if (plain.empty())
				{
					isOK = false;
					return StrConfig{};
				}

				StrConfig restored = StrConfig::deserialize(plain);

				isOK = true;
				return restored;
			}
			catch (const std::exception&)
			{
				isOK = false;
				return StrConfig{};
			}
			catch (...)
			{
				isOK = false;
				return StrConfig{};
			}
		}
	}
}

