#pragma once

#include"oso_core.h"
#include<filesystem>

namespace rw {
	namespace oso {
		class IStorageStrategy {
		public:
			virtual ~IStorageStrategy() = default;

			virtual bool save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName) = 0;
			virtual std::shared_ptr<ObjectStoreAssembly> load(const std::filesystem::path& fileName) = 0;
			virtual std::string getFormatString(const ObjectStoreAssembly& assembly) = 0;
		};
	} // namespace oso
} // namespace rw