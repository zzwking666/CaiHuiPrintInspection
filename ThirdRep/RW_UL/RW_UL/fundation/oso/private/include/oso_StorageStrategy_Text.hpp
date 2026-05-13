#pragma once

#include "oso_core.h"
#include "oso_IStorageStrategy.hpp"

#include <vector>
#include <filesystem>

namespace rw
{
	namespace oso
	{
		class StorageStrategy_Text final : public IStorageStrategy
		{
		public:
			bool save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName) override;
			std::shared_ptr<ObjectStoreAssembly> load(const std::filesystem::path& fileName) override;
			std::string getFormatString(const ObjectStoreAssembly& assembly) override;

		private:
			static void appendTxtFromStoreItem(std::ostream& os, const std::shared_ptr<ObjectStoreItem> source);
			static void appendTxtFromStoreAssembly(std::ostream& os, const std::shared_ptr<ObjectStoreAssembly> source);

		public:
			static std::string getString(const std::shared_ptr<ObjectStoreAssembly> source);

		private:
			static void appendObjectStoreItemFromTxt(std::shared_ptr<ObjectStoreAssembly> target, std::istream& is);
			static void appendObjectStoreAssemblyFromTxt(std::shared_ptr<ObjectStoreAssembly> target, std::istream& is);
			static void appendObjectStoreChildAssemblyFromTxt(std::shared_ptr<ObjectStoreAssembly> target, std::istream& is);

		public:
			static std::shared_ptr<ObjectStoreAssembly> getStoreAssemblyFromString(const std::string& source);
		};
	} // namespace oso
} // namespace rw