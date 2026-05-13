#include "oso_StorageStrategy_Json.hpp"

#include "json/json.h"

#include <fstream>
#include <sstream>

namespace rw {
	namespace oso {
		bool StorageStrategy_Json::save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName) {
			try
			{
				auto jsonString = getFormatString(assembly);
				std::ofstream file(fileName);
				if (!file.is_open()) {
					return false;
				}
				file << jsonString;
				file.close();
				return true;
			}
			catch (std::runtime_error & e)
			{
				return false;
			}
			catch (...) {
				return false;
			}

			
		}

		std::shared_ptr<ObjectStoreAssembly> StorageStrategy_Json::load(const std::filesystem::path& fileName) {
			try
			{
				std::ifstream file(fileName);
				if (!file.is_open()) {
					return nullptr;
				}
				std::stringstream buffer;
				buffer << file.rdbuf();
				file.close();
				auto assembly = getStoreAssemblyFromString(buffer.str());
				return assembly;
			}
			catch (std::runtime_error & e)
			{
				return nullptr;
			}
			catch (...) {
				return nullptr;
			}
		}

		std::string StorageStrategy_Json::getFormatString(const ObjectStoreAssembly& assembly) {
			return getString(std::make_shared<ObjectStoreAssembly>(assembly));
		}

		void StorageStrategy_Json::appendJsonValueFromStoreItem(Json::Value& jsonValue, const std::shared_ptr<ObjectStoreItem> source) {
			jsonValue["name"] = source->getName();
			jsonValue["type"] = source->getTypeString();
			switch (source->getType()) {
			case ObjectDataItemStoreType::item_bool:
				jsonValue["value"] = source->getValueAsBool();
				break;
			case ObjectDataItemStoreType::item_double:
				jsonValue["value"] = source->getValueAsDouble();
				break;
			case ObjectDataItemStoreType::item_int:
				jsonValue["value"] = source->getValueAsInt();
				break;
			case ObjectDataItemStoreType::item_float:
				jsonValue["value"] = source->getValueAsFloat();
				break;
			case ObjectDataItemStoreType::item_long:
				jsonValue["value"] = source->getValueAsLong();
				break;
			case ObjectDataItemStoreType::item_string:
				jsonValue["value"] = source->getValueAsString();
				break;
			default:
				throw std::runtime_error("Unknown value type");
			}
		}

		void StorageStrategy_Json::appendJsonValueFromStoreAssembly(Json::Value& jsonValue, const std::shared_ptr<ObjectStoreAssembly> source) {
			jsonValue["name"] = source->getName();
			Json::Value children(Json::arrayValue);
			for (const auto& item : source->getItems()) {
				Json::Value child;
				if (auto objectAssembly = ObjectStoreCoreToAssembly(item)) {
					appendJsonValueFromStoreAssembly(child, objectAssembly);
				}
				else if (auto objectItem = ObjectStoreCoreToItem(item)) {
					appendJsonValueFromStoreItem(child, objectItem);
				}
				else {
					throw std::runtime_error("Unknown type which is the superclass of objectStoreCore");
				}
				children.append(child);
			}
			jsonValue["children"] = children;
		}

		std::string StorageStrategy_Json::getString(const std::shared_ptr<ObjectStoreAssembly> source) {
			Json::Value root;
			appendJsonValueFromStoreAssembly(root, source);
			Json::StreamWriterBuilder writer;
			return Json::writeString(writer, root);
		}

		void StorageStrategy_Json::appendObjectStoreItemFromJsonValue(std::shared_ptr<ObjectStoreAssembly> target, const Json::Value& jsonValue) {
			auto child = std::make_shared<ObjectStoreItem>();
			child->setName(jsonValue["name"].asString());
			std::string type = jsonValue["type"].asString();
			if (type == "bool") {
				child->setValueFromBool(jsonValue["value"].asBool());
			}
			else if (type == "double") {
				child->setValueFromDouble(jsonValue["value"].asDouble());
			}
			else if (type == "int") {
				child->setValueFromInt(jsonValue["value"].asInt());
			}
			else if (type == "float") {
				child->setValueFromFloat(jsonValue["value"].asFloat());
			}
			else if (type == "long") {
				child->setValueFromLong(jsonValue["value"].asInt64());
			}
			else if (type == "string") {
				child->setValueFromString(jsonValue["value"].asString());
			}
			else {
				throw std::runtime_error("Unknown type");
			}
			target->addItem(child);
		}

		void StorageStrategy_Json::appendObjectStoreAssemblyFromJsonValue(std::shared_ptr<ObjectStoreAssembly> target, const Json::Value& jsonValue) {
			target->setName(jsonValue["name"].asString());
			for (const auto& child : jsonValue["children"]) {
				if (child.isMember("type")) {
					appendObjectStoreItemFromJsonValue(target, child);
				}
				else {
					auto children = std::make_shared<ObjectStoreAssembly>();
					target->addItem(children);
					appendObjectStoreAssemblyFromJsonValue(children, child);
				}
			}
		}

		std::shared_ptr<ObjectStoreItem> StorageStrategy_Json::getStoreItemFromString(const std::string& source) {
			Json::CharReaderBuilder reader;
			Json::Value root;
			std::istringstream iss(source);
			std::string errs;
			if (!Json::parseFromStream(reader, iss, &root, &errs)) {
				throw std::runtime_error("Failed to parse JSON string");
			}
			auto result = std::make_shared<ObjectStoreItem>();
			result->setName(root["name"].asString());
			std::string type = root["type"].asString();
			if (type == "bool") {
				result->setValueFromBool(root["value"].asBool());
			}
			else if (type == "double") {
				result->setValueFromDouble(root["value"].asDouble());
			}
			else if (type == "int") {
				result->setValueFromInt(root["value"].asInt());
			}
			else if (type == "float") {
				result->setValueFromFloat(root["value"].asFloat());
			}
			else if (type == "long") {
				result->setValueFromLong(root["value"].asInt64());
			}
			else if (type == "string") {
				result->setValueFromString(root["value"].asString());
			}
			else {
				throw std::runtime_error("Unknown type");
			}
			return result;
		}

		std::shared_ptr<ObjectStoreAssembly> StorageStrategy_Json::getStoreAssemblyFromString(const std::string& source) {
			Json::CharReaderBuilder reader;
			Json::Value root;
			std::istringstream iss(source);
			std::string errs;
			if (!Json::parseFromStream(reader, iss, &root, &errs)) {
				return nullptr;
			}
			auto result = std::make_shared<ObjectStoreAssembly>();
			appendObjectStoreAssemblyFromJsonValue(result, root);
			return result;
		}
	} // namespace oso
} // namespace rw