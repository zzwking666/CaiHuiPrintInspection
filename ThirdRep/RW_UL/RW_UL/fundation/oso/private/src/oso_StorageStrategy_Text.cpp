#include "oso_StorageStrategy_Text.hpp"

#include <sstream>
#include <fstream>

namespace rw
{
	namespace oso
	{
		bool StorageStrategy_Text::save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName)
		{
			auto txtString = getFormatString(assembly);
			std::ofstream outFile(fileName);
			if (!outFile)
			{
				return false;
			}
			outFile << txtString;

			return true;
		}

		std::shared_ptr<ObjectStoreAssembly> StorageStrategy_Text::load(const std::filesystem::path& fileName)
		{
			std::ifstream inFile(fileName);
			if (!inFile)
			{
				return nullptr;
			}
			std::ostringstream oss;
			oss << inFile.rdbuf();
			std::string str = oss.str();
			auto assembly = getStoreAssemblyFromString(str);
			return assembly;
		}

		std::string StorageStrategy_Text::getFormatString(const ObjectStoreAssembly& assembly)
		{
			return getString(std::make_shared<ObjectStoreAssembly>(assembly));
		}

		void StorageStrategy_Text::appendTxtFromStoreItem(std::ostream& os, const std::shared_ptr<ObjectStoreItem> source)
		{
			os << "ObjectStoreItem\n";
			os << "name: " << source->getName() << "\n";
			os << "type: " << source->getTypeString() << "\n";

			switch (source->getType())
			{
			case ObjectDataItemStoreType::item_bool:
				os << "value: " << (source->getValueAsBool() ? "true" : "false") << "\n";
				break;
			case ObjectDataItemStoreType::item_double:
				os << "value: " << source->getValueAsDouble() << "\n";
				break;
			case ObjectDataItemStoreType::item_int:
				os << "value: " << source->getValueAsInt() << "\n";
				break;
			case ObjectDataItemStoreType::item_float:
				os << "value: " << source->getValueAsFloat() << "\n";
				break;
			case ObjectDataItemStoreType::item_long:
				os << "value: " << source->getValueAsLong() << "\n";
				break;
			case ObjectDataItemStoreType::item_string:
				os << "value: " << source->getValueAsString() << "\n";
				break;
			default:
				throw std::runtime_error("Unknown type");
			}
		}

		void StorageStrategy_Text::appendTxtFromStoreAssembly(std::ostream& os, const std::shared_ptr<ObjectStoreAssembly> source)
		{
			os << "ObjectStoreAssembly\n";
			os << "name: " << source->getName() << "\n";
			os << "children:\n";
			os << "size: " << source->getItems().size() << "\n";
			for (const auto& item : source->getItems())
			{
				if (auto objectAssembly = ObjectStoreCoreToAssembly(item))
				{
					appendTxtFromStoreAssembly(os, objectAssembly);
				}
				else if (auto objectItem = ObjectStoreCoreToItem(item))
				{
					appendTxtFromStoreItem(os, objectItem);
				}
				else
				{
					throw std::runtime_error("Unknown type which is the superclass of objectStoreCore");
				}
			}
		}

		std::string StorageStrategy_Text::getString(const std::shared_ptr<ObjectStoreAssembly> source)
		{
			std::ostringstream oss;
			appendTxtFromStoreAssembly(oss, source);
			return oss.str();
		}

		void StorageStrategy_Text::appendObjectStoreItemFromTxt(std::shared_ptr<ObjectStoreAssembly> target, std::istream& is)
		{
			auto child = std::make_shared<ObjectStoreItem>();
			std::string line;
			std::getline(is, line); // Read name
			child->setName(line.substr(line.find(": ") + 2));
			std::getline(is, line); // Read type
			std::string type = line.substr(line.find(": ") + 2);
			std::getline(is, line); // Read value
			std::string value = line.substr(line.find(": ") + 2);

			if (type == "bool")
			{
				child->setValueFromBool(value == "true");
			}
			else if (type == "double")
			{
				child->setValueFromDouble(std::stod(value));
			}
			else if (type == "int")
			{
				child->setValueFromInt(std::stoi(value));
			}
			else if (type == "float")
			{
				child->setValueFromFloat(std::stof(value));
			}
			else if (type == "long")
			{
				child->setValueFromLong(std::stol(value));
			}
			else if (type == "string")
			{
				child->setValueFromString(value);
			}
			else
			{
				throw std::runtime_error("Unknown type");
			}

			target->addItem(child);
		}

		void StorageStrategy_Text::appendObjectStoreAssemblyFromTxt(std::shared_ptr<ObjectStoreAssembly> target, std::istream& is)
		{
			std::string line;
			std::getline(is, line); // Skip "ObjectStoreAssembly"
			std::getline(is, line); // Read name
			target->setName(line.substr(line.find(": ") + 2));
			std::getline(is, line); // Skip "children:"
			std::getline(is, line); // Read size
			int size = std::stoi(line.substr(line.find(": ") + 2));

			for (int i = 0; i < size; ++i)
			{
				std::getline(is, line);
				if (line == "ObjectStoreItem")
				{
					appendObjectStoreItemFromTxt(target, is);
				}
				else if (line == "ObjectStoreAssembly")
				{
					auto children = std::make_shared<ObjectStoreAssembly>();
					target->addItem(children);
					appendObjectStoreChildAssemblyFromTxt(children, is);
				}
				else
				{
					throw std::runtime_error("Unknown type");
				}
			}
		}

		void StorageStrategy_Text::appendObjectStoreChildAssemblyFromTxt(std::shared_ptr<ObjectStoreAssembly> target,
			std::istream& is)
		{
			std::string line;
			std::getline(is, line); // Read name
			target->setName(line.substr(line.find(": ") + 2));
			std::getline(is, line); // Skip "children:"
			std::getline(is, line); // Read size
			int size = std::stoi(line.substr(line.find(": ") + 2));

			for (int i = 0; i < size; ++i)
			{
				std::getline(is, line);
				if (line == "ObjectStoreItem")
				{
					appendObjectStoreItemFromTxt(target, is);
				}
				else if (line == "ObjectStoreAssembly")
				{
					auto children = std::make_shared<ObjectStoreAssembly>();
					target->addItem(children);
					appendObjectStoreChildAssemblyFromTxt(children, is);
				}
				else
				{
					throw std::runtime_error("Unknown type");
				}
			}
		}

		std::shared_ptr<ObjectStoreAssembly> StorageStrategy_Text::getStoreAssemblyFromString(const std::string& source)
		{
			std::istringstream iss(source);
			auto result = std::make_shared<ObjectStoreAssembly>();
			appendObjectStoreAssemblyFromTxt(result, iss);
			return result;
		}
	} // namespace oso
} // namespace rw