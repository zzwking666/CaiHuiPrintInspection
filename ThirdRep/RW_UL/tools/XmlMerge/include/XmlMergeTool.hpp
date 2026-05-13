#pragma once
#include <memory>
#include <filesystem>
#include <string>

#include "oso_core.h"
#include "oso_StorageContext.hpp"

using Type = rw::oso::ObjectDataItemStoreType;

namespace zzw
{
	namespace XmlMerge
	{
		struct XmlMergeTool
		{
		public:
			static void Merge(rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly);
			static rw::oso::ObjectStoreAssembly Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly);
			static rw::oso::ObjectStoreAssembly Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly,bool& isSuccess);
		private:
			static void MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly);
			static void MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly,bool& isSuccess);

		};
	}// namespace zzw::XmlMerge
} 