#pragma once

#include"oso_core.h"
#include"oso_IStorageStrategy.hpp"

#include<vector>

namespace pugi
{
	class xml_node;
}

namespace rw
{
	namespace oso
	{
		class StorageStrategy_Xml final
			: public IStorageStrategy
		{
		public:
			bool save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName) override;
			std::shared_ptr<ObjectStoreAssembly> load(const std::filesystem::path& fileName) override;
			std::string getFormatString(const ObjectStoreAssembly& assembly) override;
		private:
			static void appendXmlNodeFromStoreItem(pugi::xml_node& node, const std::shared_ptr<ObjectStoreItem> source);
			static void appendXmlNodeFromStoreAssembly(pugi::xml_node& node, const std::shared_ptr<ObjectStoreAssembly> source);
		public:
			static std::string getString(const std::shared_ptr<ObjectStoreAssembly> source);
		private:
			static void appendObjectStoreItemFromXmlNode(std::shared_ptr<ObjectStoreAssembly> target, const pugi::xml_node& node);
			static void appendObjectStoreAssemblyFromXmlNode(std::shared_ptr<ObjectStoreAssembly> target, const pugi::xml_node& node);
		public:
			static std::shared_ptr<ObjectStoreItem> getStoreItemFromString(const std::string& source);
			static std::shared_ptr<ObjectStoreAssembly> getStoreAssemblyFromString(const std::string& source);
		};
	} // namespace oso
} // namespace rw