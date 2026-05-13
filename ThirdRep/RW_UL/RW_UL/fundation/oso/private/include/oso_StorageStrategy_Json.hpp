#pragma once

#include "oso_IStorageStrategy.hpp"
#include "json/json.h"

namespace rw {
	namespace oso {
		class StorageStrategy_Json final : public IStorageStrategy {
		public:
			bool save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName) override;
			std::shared_ptr<ObjectStoreAssembly> load(const std::filesystem::path& fileName) override;
			std::string getFormatString(const ObjectStoreAssembly& assembly) override;

		private:
			static void appendJsonValueFromStoreItem(Json::Value& jsonValue, const std::shared_ptr<ObjectStoreItem> source);
			static void appendJsonValueFromStoreAssembly(Json::Value& jsonValue, const std::shared_ptr<ObjectStoreAssembly> source);

		public:
			static std::string getString(const std::shared_ptr<ObjectStoreAssembly> source);

		private:
			static void appendObjectStoreItemFromJsonValue(std::shared_ptr<ObjectStoreAssembly> target, const Json::Value& jsonValue);
			static void appendObjectStoreAssemblyFromJsonValue(std::shared_ptr<ObjectStoreAssembly> target, const Json::Value& jsonValue);

		public:
			static std::shared_ptr<ObjectStoreItem> getStoreItemFromString(const std::string& source);
			static std::shared_ptr<ObjectStoreAssembly> getStoreAssemblyFromString(const std::string& source);
		};
	} // namespace oso
} // namespace rw