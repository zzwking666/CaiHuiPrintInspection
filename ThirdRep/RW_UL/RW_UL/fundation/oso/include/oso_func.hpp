#pragma once

#include "oso_core.h"
#include "oso_StorageContext.hpp"

namespace rw
{
	namespace oso
	{
		struct AssemblyMergeTool
		{
		private:
			using Type = rw::oso::ObjectDataItemStoreType;
		public:
			static void Merge(rw::oso::ObjectStoreAssembly& targetAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly);
			static rw::oso::ObjectStoreAssembly Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly);
			static rw::oso::ObjectStoreAssembly Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly, bool& isSuccess);
		private:
			static void MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly);
			static void MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
				const rw::oso::ObjectStoreAssembly& oldAssembly, bool& isSuccess);

		};
	}
}