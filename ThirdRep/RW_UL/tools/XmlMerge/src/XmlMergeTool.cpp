#include "XmlMergeTool.hpp"
#include <algorithm>

namespace zzw
{
	namespace XmlMerge
	{
		void XmlMergeTool::Merge(rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly)
		{
			MergeAssembly(newAssembly, oldAssembly);
		}

		rw::oso::ObjectStoreAssembly XmlMergeTool::Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly)
		{
			rw::oso::ObjectStoreAssembly mergedAssembly = newAssembly;
			MergeAssembly(mergedAssembly, oldAssembly);
			return mergedAssembly;
		}

		rw::oso::ObjectStoreAssembly XmlMergeTool::Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly, bool& isSuccess)
		{
			rw::oso::ObjectStoreAssembly mergedAssembly = newAssembly;
			isSuccess = true;
			MergeAssembly(mergedAssembly, oldAssembly, isSuccess);
			return mergedAssembly;
		}

		void XmlMergeTool::MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
		                                 const rw::oso::ObjectStoreAssembly& oldAssembly)
		{
			if ("assembly" == newAssembly.getStoreType() &&
				"assembly" == oldAssembly.getStoreType())
			{
				auto newCoreVector = newAssembly.getItems();
				auto oldCoreVector = oldAssembly.getItems();

				for (auto& newCore : newCoreVector)
				{
					if ("core" == newCore->getStoreType())
					{
						auto newCoreName = newCore->getName();
						for (const auto& oldCore : oldCoreVector)
						{
							if ("core" == oldCore->getStoreType())
							{
								auto oldCoreName = oldCore->getName();

								if (newCoreName == oldCoreName)
								{
									auto newItem = rw::oso::ObjectStoreCoreToItem(newCore);
									auto oldItem = rw::oso::ObjectStoreCoreToItem(oldCore);
									if ("item" == newItem->getStoreType() &&
										"item" == oldItem->getStoreType())
									{
										auto newItemType = newItem->getType();
										auto oldItemType = oldItem->getType();
										bool isTypeSame = false;

										if (newItemType == oldItemType)
										{
											isTypeSame = true;
										}

										switch (newItemType)
										{
										case Type::item_string:
											if (isTypeSame)
											{
												newItem->setValueFromString(oldItem->getValueAsString());
											}
											else
											{
												newItem->setValueFromString("");
											}
											break;
										case Type::item_int:
											if (isTypeSame)
											{
												newItem->setValueFromInt(oldItem->getValueAsInt());
											}
											else
											{
												newItem->setValueFromInt(0);
											}
											break;
										case Type::item_float:
											if (isTypeSame)
											{
												newItem->setValueFromFloat(oldItem->getValueAsFloat());
											}
											else
											{
												newItem->setValueFromFloat(0.0f);
											}
											break;
										case Type::item_double:
											if (isTypeSame)
											{
												newItem->setValueFromDouble(oldItem->getValueAsDouble());
											}
											else
											{
												newItem->setValueFromDouble(0.0);
											}
											break;
										case Type::item_bool:
											if (isTypeSame)
											{
												newItem->setValueFromBool(oldItem->getValueAsBool());
											}
											else
											{
												newItem->setValueFromBool(false);
											}
											break;
										case Type::item_long:
											if (isTypeSame)
											{
												newItem->setValueFromLong(oldItem->getValueAsLong());
											}
											else
											{
												newItem->setValueFromLong(0L);
											}
											break;
										default:
											break;
										}

										break;
									}
								}
							}
						}
					}
				}
			}
		}

		void XmlMergeTool::MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly, bool& isSuccess)
		{
			if ("assembly" == newAssembly.getStoreType() &&
				"assembly" == oldAssembly.getStoreType())
			{
				auto newCoreVector = newAssembly.getItems();
				auto oldCoreVector = oldAssembly.getItems();

				for (auto& newCore : newCoreVector)
				{
					if ("core" == newCore->getStoreType())
					{
						auto newCoreName = newCore->getName();
						for (const auto& oldCore : oldCoreVector)
						{
							if ("core" == oldCore->getStoreType())
							{
								auto oldCoreName = oldCore->getName();

								if (newCoreName == oldCoreName)
								{
									auto newItem = rw::oso::ObjectStoreCoreToItem(newCore);
									auto oldItem = rw::oso::ObjectStoreCoreToItem(oldCore);
									if ("item" == newItem->getStoreType() &&
										"item" == oldItem->getStoreType())
									{
										auto newItemType = newItem->getType();
										auto oldItemType = oldItem->getType();
										bool isTypeSame = false;

										if (newItemType == oldItemType)
										{
											isTypeSame = true;
										}

										switch (newItemType)
										{
										case Type::item_string:
											if (isTypeSame)
											{
												newItem->setValueFromString(oldItem->getValueAsString());
											}
											else
											{
												newItem->setValueFromString("");
											}
											break;
										case Type::item_int:
											if (isTypeSame)
											{
												newItem->setValueFromInt(oldItem->getValueAsInt());
											}
											else
											{
												newItem->setValueFromInt(0);
											}
											break;
										case Type::item_float:
											if (isTypeSame)
											{
												newItem->setValueFromFloat(oldItem->getValueAsFloat());
											}
											else
											{
												newItem->setValueFromFloat(0.0f);
											}
											break;
										case Type::item_double:
											if (isTypeSame)
											{
												newItem->setValueFromDouble(oldItem->getValueAsDouble());
											}
											else
											{
												newItem->setValueFromDouble(0.0);
											}
											break;
										case Type::item_bool:
											if (isTypeSame)
											{
												newItem->setValueFromBool(oldItem->getValueAsBool());
											}
											else
											{
												newItem->setValueFromBool(false);
											}
											break;
										case Type::item_long:
											if (isTypeSame)
											{
												newItem->setValueFromLong(oldItem->getValueAsLong());
											}
											else
											{
												newItem->setValueFromLong(0L);
											}
											break;
										default:
											break;
										}

										break;
									}
									else
									{
										isSuccess = false;
									}
								}
							}
						}
					}
					else
					{
						isSuccess = false;
					}
				}
			}
			else
			{
				isSuccess = false;
			}
		}
	}
} // namespace zzw::XmlMerge