#include"oso_StorageStrategy_xml.hpp"

#include"pugixml.hpp"

#include<sstream>

namespace rw
{
	namespace oso
	{
		bool StorageStrategy_Xml::save(const ObjectStoreAssembly& assembly, const std::filesystem::path& fileName)
		{
			try
			{
				auto xmlString = getFormatString(assembly);
				pugi::xml_document doc;
				if (auto loadResult = doc.load_string(xmlString.c_str()); !loadResult) {
					return false;
				}
				if (auto saveResult = doc.save_file(fileName.c_str()); !saveResult) {
					return false;
				}
				return true;
			}
			catch (std::runtime_error & e)
			{
				return false;
			}
			catch (...)
			{
				return nullptr;
			}
			
		}

		std::shared_ptr<ObjectStoreAssembly> StorageStrategy_Xml::load(const std::filesystem::path& fileName)
		{
			try
			{
				pugi::xml_document doc;
				auto loadResult = doc.load_file(fileName.string().c_str());
				if (!loadResult) {
					return nullptr;
				}
				auto root = doc.root();
				std::ostringstream oss;
				root.print(oss);
				std::string str = oss.str();
				auto assembly = getStoreAssemblyFromString(str);
				return assembly;
			}
			catch (std::runtime_error & e)
			{
				return nullptr;
			}
			catch (...)
			{
				return nullptr;
			}
		}

		std::string StorageStrategy_Xml::getFormatString(const ObjectStoreAssembly& assembly)
		{
			return getString(std::make_shared<ObjectStoreAssembly>(assembly));
		}

		void StorageStrategy_Xml::appendXmlNodeFromStoreItem(pugi::xml_node& node,
			const std::shared_ptr<ObjectStoreItem> source)
		{
			node.set_name("ObjectStoreItem");
			node.append_attribute("name").set_value(source->getName().c_str());

			if (source->getType() == ObjectDataItemStoreType::item_bool) {
				node.append_child("value").text().set(source->getValueAsBool() ? "true" : "false");
				node.append_child("type").text().set("bool");
			}
			else if (source->getType() == ObjectDataItemStoreType::item_double) {
				node.append_child("value").text().set(std::to_string(source->getValueAsDouble()).c_str());
				node.append_child("type").text().set("double");
			}
			else if (source->getType() == ObjectDataItemStoreType::item_int) {
				node.append_child("value").text().set(std::to_string(source->getValueAsInt()).c_str());
				node.append_child("type").text().set("int");
			}
			else if (source->getType() == ObjectDataItemStoreType::item_float) {
				node.append_child("value").text().set(std::to_string(source->getValueAsFloat()).c_str());
				node.append_child("type").text().set("float");
			}
			else if (source->getType() == ObjectDataItemStoreType::item_long) {
				node.append_child("value").text().set(std::to_string(source->getValueAsLong()).c_str());
				node.append_child("type").text().set("long");
			}
			else if (source->getType() == ObjectDataItemStoreType::item_string) {
				node.append_child("value").text().set(source->getValueAsString().c_str());
				node.append_child("type").text().set("string");
			}
			else {
				throw std::runtime_error("Unknown value type");
			}
		}

		void StorageStrategy_Xml::appendXmlNodeFromStoreAssembly(pugi::xml_node& node,
			const std::shared_ptr<ObjectStoreAssembly> source)
		{
			node.set_name("ObjectStoreAssembly");
			node.append_attribute("name").set_value(source->getName().c_str());

			auto children = node.append_child("children");
			for (const auto& item : source->getItems()) {
				if (auto objectAssembly = ObjectStoreCoreToAssembly(item)) {
					auto child = children.append_child();
					appendXmlNodeFromStoreAssembly(child, objectAssembly);
				}
				else if (auto objectItem = ObjectStoreCoreToItem(item)) {
					auto child = children.append_child();
					appendXmlNodeFromStoreItem(child, objectItem);
				}
				else {
					throw std::runtime_error("Unknown type which is the superclass of objectStoreCore");
				}
			}
		}

		std::string StorageStrategy_Xml::getString(const std::shared_ptr<ObjectStoreAssembly> source)
		{
			pugi::xml_document doc;
			pugi::xml_node root = doc.append_child("root");
			appendXmlNodeFromStoreAssembly(root, source);

			std::ostringstream oss;
			root.print(oss);
			std::string result = oss.str();

			return result;
		}
		void StorageStrategy_Xml::appendObjectStoreItemFromXmlNode(std::shared_ptr<ObjectStoreAssembly> target, const pugi::xml_node& node)
		{
			auto child = std::make_shared<ObjectStoreItem>();
			child->setName(node.attribute("name").as_string());
			if (std::string(node.child("type").text().as_string()) == "bool") {
				child->setValueFromBool(std::string(node.child("value").text().as_string()) == "true");
			}
			else if (std::string(node.child("type").text().as_string()) == "double") {
				child->setValueFromDouble(node.child("value").text().as_double());
			}
			else if (std::string(node.child("type").text().as_string()) == "int") {
				child->setValueFromInt(node.child("value").text().as_int());
			}
			else if (std::string(node.child("type").text().as_string()) == "float") {
				child->setValueFromFloat(node.child("value").text().as_float());
			}
			else if (std::string(node.child("type").text().as_string()) == "long") {
				child->setValueFromLong(node.child("value").text().as_llong());
			}
			else if (std::string(node.child("type").text().as_string()) == "string") {
				child->setValueFromString(node.child("value").text().as_string());
			}
			else {
				throw std::runtime_error("Unknown type");
			}

			target->addItem(child);
		}

		void StorageStrategy_Xml::appendObjectStoreAssemblyFromXmlNode(std::shared_ptr<ObjectStoreAssembly> target,
			const pugi::xml_node& node)
		{
			target->setName(node.attribute("name").as_string());
			for (const auto& child : node.child("children").children()) {
				if (std::string(child.name()) == "ObjectStoreItem") {
					appendObjectStoreItemFromXmlNode(target, child);
				}
				else if (std::string(child.name()) == "ObjectStoreAssembly") {
					auto children = std::make_shared<ObjectStoreAssembly>();
					target->addItem(children);
					appendObjectStoreAssemblyFromXmlNode(children, child);
				}
				else {
					throw std::runtime_error("Unknown type");
				}
			}
		}

		std::shared_ptr<ObjectStoreItem> StorageStrategy_Xml::getStoreItemFromString(const std::string& source)
		{
			pugi::xml_document doc;
			auto loadResult = doc.load_string(source.c_str());
			if (!loadResult) {
				throw std::runtime_error("Failed to load string to xml document");
			}

			auto result = std::make_shared<ObjectStoreItem>();
			auto root = doc.child("ObjectStoreItem");
			if (!root) {
				throw std::runtime_error("Failed to find root node");
			}

			result->setName(root.attribute("name").as_string());
			if (std::string(root.child("type").text().as_string()) == "bool") {
				result->setValueFromBool(std::string(root.child("value").text().as_string()) == "true");
			}
			else if (std::string(root.child("type").text().as_string()) == "double") {
				result->setValueFromDouble(root.child("value").text().as_double());
			}
			else if (std::string(root.child("type").text().as_string()) == "int") {
				result->setValueFromInt(root.child("value").text().as_int());
			}
			else if (std::string(root.child("type").text().as_string()) == "float") {
				result->setValueFromFloat(root.child("value").text().as_float());
			}
			else if (std::string(root.child("type").text().as_string()) == "long") {
				result->setValueFromLong(root.child("value").text().as_llong());
			}
			else if (std::string(root.child("type").text().as_string()) == "string") {
				result->setValueFromString(root.child("value").text().as_string());
			}
			else {
				throw std::runtime_error("Unknown type");
			}

			return result;
		}

		std::shared_ptr<ObjectStoreAssembly> StorageStrategy_Xml::getStoreAssemblyFromString(const std::string& source)
		{
			pugi::xml_document doc;
			auto loadResult = doc.load_string(source.c_str());
			if (!loadResult) {
				return nullptr;
			}

			auto result = std::make_shared<ObjectStoreAssembly>();
			auto root = doc.child("ObjectStoreAssembly");
			if (!root) {
				return nullptr;
			}

			appendObjectStoreAssemblyFromXmlNode(result, root);

			return result;
		}
	} // namespace oso
} // namespace rw