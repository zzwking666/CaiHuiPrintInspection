#include"imgPro_IndexFunc.hpp"

namespace rw
{
	namespace imgPro
	{
		ProcessResultIndexMap IndexFunc::getIndex(const ProcessResult& info)
		{
			std::unordered_map<ClassId, std::set<ProcessResultIndex>> result;

			for (size_t i = 0; i < info.size(); ++i)
			{
				result[info[i].classId].insert(i);
			}

			return result;
		}

		std::vector<ProcessResultIndex> IndexFunc::removeIndicesIf(ProcessResultIndexMap& indexMap,
		                                                           const RemoveIndicesIf& predicate, ImageProcessContext& context)
		{
			std::vector<ProcessResultIndex> removed;
			for (auto& kv : indexMap)
			{
				auto& indices = kv.second;
				for (auto it = indices.begin(); it != indices.end(); )
				{
					if (predicate(kv.first, *it, context))
					{
						removed.push_back(*it);
						it = indices.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			return removed;
		}

		std::vector<ProcessResultIndex> IndexFunc::removeIndicesIfByInfo(ProcessResultIndexMap& indexMap,
		                                                                 const ProcessResult& info,
		                                                                 const RemoveIndicesIfByInfo& predicate, ImageProcessContext& context)
		{
			std::vector<ProcessResultIndex> removed;
			for (auto& kv : indexMap)
			{
				auto& indices = kv.second;
				for (auto it = indices.begin(); it != indices.end(); )
				{
					ProcessResultIndex idx = *it;
					if (idx < info.size() && predicate(info[idx], context))
					{
						removed.push_back(idx);
						it = indices.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			return removed;
		}
	}
}