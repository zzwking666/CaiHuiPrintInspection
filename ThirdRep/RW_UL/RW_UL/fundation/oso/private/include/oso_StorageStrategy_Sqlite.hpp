#pragma once

#include "oso_IStorageStrategy.hpp"
#include "sqlite3.h"
#include <filesystem>
#include <memory>
#include <string>

namespace rw {
	namespace oso {
		class StorageStrategy_Sqlite final : public IStorageStrategy {
		public:
			bool save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName) override;
			std::shared_ptr<ObjectStoreAssembly> load(const std::filesystem::path& fileName) override;
			std::string getFormatString(const ObjectStoreAssembly& assembly) override;

		private:
			static void saveItem(sqlite3* db, const std::shared_ptr<ObjectStoreItem> item, int parentId);
			static void saveAssembly(sqlite3* db, const std::shared_ptr<ObjectStoreAssembly> assembly, int parentId);

		public:
			static std::string getString(const std::shared_ptr<ObjectStoreAssembly> source);

		private:
			static void loadItem(sqlite3* db, std::shared_ptr<ObjectStoreAssembly> target, int parentId);
			static void loadAssembly(sqlite3* db, std::shared_ptr<ObjectStoreAssembly> target, int parentId);
		};
	} // namespace oso
} // namespace rw