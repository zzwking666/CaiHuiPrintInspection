#include"oso_func.hpp"

namespace rw
{
	namespace oso
	{
		void AssemblyMergeTool::Merge(rw::oso::ObjectStoreAssembly& targetAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly)
		{
			MergeAssembly(targetAssembly, oldAssembly);
		}

		rw::oso::ObjectStoreAssembly AssemblyMergeTool::Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly)
		{
			rw::oso::ObjectStoreAssembly mergedAssembly = newAssembly;
			MergeAssembly(mergedAssembly, oldAssembly);
			return mergedAssembly;
		}

		rw::oso::ObjectStoreAssembly AssemblyMergeTool::Merge(const rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly, bool& isSuccess)
		{
			rw::oso::ObjectStoreAssembly mergedAssembly = newAssembly;
			isSuccess = true;
			MergeAssembly(mergedAssembly, oldAssembly, isSuccess);
			return mergedAssembly;
		}

		void AssemblyMergeTool::MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly)
		{
			if ("assembly" == newAssembly.getStoreType() &&
				"assembly" == oldAssembly.getStoreType())
			{
				auto newCoreVector = newAssembly.getItems();
				auto oldCoreVector = oldAssembly.getItems();

				for (auto& newCore : newCoreVector)
				{
					if ("item" == newCore->getStoreType())
					{
						auto newCoreName = newCore->getName();
						for (const auto& oldCore : oldCoreVector)
						{
							if ("item" == oldCore->getStoreType())
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
								else
								{
									auto newItem = rw::oso::ObjectStoreCoreToItem(newCore);

									auto newItemType = newItem->getType();

									switch (newItemType)
									{
									case Type::item_string:
										if ("0" == newItem->getValueAsString())
										{
											newItem->setValueFromString("0");
										}
										break;
									case Type::item_int:
										if (0 == newItem->getValueAsInt())
										{
											newItem->setValueFromInt(0);
										}
										break;
									case Type::item_float:
										if (0 == newItem->getValueAsFloat())
										{
											newItem->setValueFromFloat(0);
										}
										break;
									case Type::item_double:
										if (0 == newItem->getValueAsDouble())
										{
											newItem->setValueFromDouble(0);
										}
										break;
									case Type::item_bool:
										if (0 == newItem->getValueAsBool())
										{
											newItem->setValueFromBool(false);
										}
										break;
									case Type::item_long:
										if (0 == newItem->getValueAsLong())
										{
											newItem->setValueFromLong(0);
										}
										break;
									default:
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		void AssemblyMergeTool::MergeAssembly(rw::oso::ObjectStoreAssembly& newAssembly,
			const rw::oso::ObjectStoreAssembly& oldAssembly, bool& isSuccess)
		{
			if ("assembly" == newAssembly.getStoreType() &&
				"assembly" == oldAssembly.getStoreType())
			{
				auto newCoreVector = newAssembly.getItems();
				auto oldCoreVector = oldAssembly.getItems();

				for (auto& newCore : newCoreVector)
				{
					if ("item" == newCore->getStoreType())
					{
						auto newCoreName = newCore->getName();
						for (const auto& oldCore : oldCoreVector)
						{
							if ("item" == oldCore->getStoreType())
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
								else
								{
									auto newItem = rw::oso::ObjectStoreCoreToItem(newCore);

									auto newItemType = newItem->getType();

									switch (newItemType)
									{
									case Type::item_string:
										if ("0" == newItem->getValueAsString())
										{
											newItem->setValueFromString("0");
										}
										break;
									case Type::item_int:
										if (0 == newItem->getValueAsInt())
										{
											newItem->setValueFromInt(0);
										}
										break;
									case Type::item_float:
										if (0 == newItem->getValueAsFloat())
										{
											newItem->setValueFromFloat(0);
										}
										break;
									case Type::item_double:
										if (0 == newItem->getValueAsDouble())
										{
											newItem->setValueFromDouble(0);
										}
										break;
									case Type::item_bool:
										if (0 == newItem->getValueAsBool())
										{
											newItem->setValueFromBool(false);
										}
										break;
									case Type::item_long:
										if (0 == newItem->getValueAsLong())
										{
											newItem->setValueFromLong(0);
										}
										break;
									default:
										break;
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

}
