#include"oso_core.h"

#include <random>

using namespace rw::oso;

namespace oso_core {
	TEST(ObjectStoreCore_Class, attribute_name) {
		ObjectStoreCore testObj;

		string name = "testName";
		string errorname = "errorName";

		testObj.setName(name);
		EXPECT_EQ(testObj.getName(), name);

		EXPECT_NE(testObj.getName(), errorname);
	}

	TEST(ObjectStoreCore_Class, attribute_level) {
		ObjectStoreCore testObj;

		auto currentLevel = testObj.level;

		//Check if the initial value is 0
		ASSERT_EQ(testObj.level, 0);

		//Check if the level should never be a positive value
		testObj.subLevel();
		ASSERT_EQ(testObj.level, 0) << "level do not <0";

		testObj.addLevel();
		EXPECT_EQ(testObj.level, currentLevel + 1);

		testObj.subLevel();
		EXPECT_EQ(testObj.level, currentLevel);

		//Randomly add specified levels
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 100);
		int random_number = dis(gen);

		for (int i = 0; i < random_number; ++i) {
			testObj.addLevel();
		}

		EXPECT_EQ(testObj.level, random_number + currentLevel);
	}

	TEST(ObjectStoreCore_Class, attribute_storeType) {
		ObjectStoreCore testObj;

		string storeType = "core";
		string errorStoreType = "error";

		EXPECT_EQ(testObj.getStoreType(), storeType);
		EXPECT_NE(testObj.getStoreType(), errorStoreType);
	}

	TEST(ObjectStoreCore_Class, ConstructorCopy)
	{
		ObjectStoreCore testObj;
		testObj.setName("testName");
		ObjectStoreCore testObj1(testObj);
		EXPECT_EQ(testObj1.getName(), "testName");
	}

	TEST(ObjectStoreCore_Class, ConstructorMove)
	{
		ObjectStoreCore testObj;
		testObj.setName("testName");
		ObjectStoreCore testObj1(std::move(testObj));
		EXPECT_EQ(testObj1.getName(), "testName");
	}

	TEST(ObjectStoreCore_Class, operatorAssignMove)
	{
		ObjectStoreCore testObj;
		testObj.setName("testName");
		ObjectStoreCore testObj1;
		testObj1 = std::move(testObj);
		EXPECT_EQ(testObj1.getName(), "testName");
	}

	TEST(ObjectStoreCore_Class, AssignmentOperator)
	{
		ObjectStoreCore testObj;
		testObj.setName("testName");
		ObjectStoreCore testObj1;
		testObj1 = testObj;
		EXPECT_EQ(testObj1.getName(), "testName");
	}

	TEST(ObjectStoreCore_Class, opeatorEuqa_true) {
		ObjectStoreCore testCore;
		ObjectStoreCore testCore1;
		testCore.setName("testName");
		testCore1.setName("testName");
		auto result = testCore == testCore1;
		EXPECT_EQ(result, true);
	}

	TEST(ObjectStoreCore_Class, opeatorEuqa_false) {
		ObjectStoreCore testCore;
		ObjectStoreCore testCore1;
		testCore.setName("testName");
		testCore1.setName("ErrorName");
		auto result = testCore == testCore1;
		EXPECT_EQ(result, false);
	}

	TEST(ObjectStoreCore_Class, opeatorNeEuqa_true) {
		ObjectStoreCore testObj;
		ObjectStoreCore testObj1;
		testObj.setName("testName");
		testObj1.setName("errorName");
		auto result = testObj != testObj1;
		EXPECT_EQ(result, true);
	}

	TEST(ObjectStoreCore_Class, opeatorNeEuqa_false) {
		ObjectStoreCore testObj;
		ObjectStoreCore testObj1;
		testObj.setName("testName");
		testObj1.setName("testName");
		auto result = testObj != testObj1;
		EXPECT_EQ(result, false);
	}
}

namespace oso_core
{
	TEST(ObjectStoreItem_Class, CopyConstructor)
	{
		ObjectStoreItem item;
		item.setName("testName");
		item.setValueFromDouble(3.14);
		ObjectStoreItem item1(item);

		EXPECT_EQ(item1.getName(), "testName");
		EXPECT_EQ(item1.getType(), ObjectDataItemStoreType::item_double);
		EXPECT_EQ(item1.getValueAsDouble(), 3.14);
	}

	TEST(ObjectStoreItem_Class, ConstructorMove)
	{
		ObjectStoreItem item;
		item.setName("testName");
		item.setValueFromDouble(3.14);
		ObjectStoreItem item1(std::move(item));
		EXPECT_EQ(item1.getName(), "testName");
		EXPECT_EQ(item1.getType(), ObjectDataItemStoreType::item_double);
		EXPECT_EQ(item1.getValueAsDouble(), 3.14);
	}

	TEST(ObjectStoreItem_Class, operatorAssignMove)
	{
		ObjectStoreItem item;
		item.setName("testName");
		item.setValueFromDouble(3.14);
		ObjectStoreItem item1;
		item1 = std::move(item);
		EXPECT_EQ(item1.getName(), "testName");
		EXPECT_EQ(item1.getType(), ObjectDataItemStoreType::item_double);
		EXPECT_EQ(item1.getValueAsDouble(), 3.14);
	}

	TEST(ObjectStoreItem_Class, AssignmentOperator)
	{
		ObjectStoreItem item;
		item.setName("testName");
		item.setValueFromDouble(3.14);
		ObjectStoreItem item1;
		item1 = item;
		EXPECT_EQ(item1.getName(), "testName");
		EXPECT_EQ(item1.getType(), ObjectDataItemStoreType::item_double);
		EXPECT_EQ(item1.getValueAsDouble(), 3.14);
	}

	TEST(ObjectStoreItem_Class, attribute_storeType) {
		ObjectStoreItem testObj;
		EXPECT_EQ(testObj.getStoreType(), "item");
	}

	TEST(ObjectStoreItem_Class, operate_setValueFromString) {
		ObjectStoreItem testObj;

		string value = "string";
		string errorValue = "error";

		testObj.setValueFromString(value);
		EXPECT_EQ(testObj.getValueAsString(), value);
		EXPECT_EQ(testObj.getType(), ObjectDataItemStoreType::item_string);
	}

	TEST(ObjectStoreItem_Class, operate_setValueFromint) {
		ObjectStoreItem testObj;

		int value = 12;
		int errorValue = 0;

		testObj.setValueFromInt(value);
		EXPECT_EQ(testObj.getValueAsInt(), value);
		EXPECT_EQ(testObj.getType(), ObjectDataItemStoreType::item_int);
	}

	TEST(ObjectStoreItem_Class, operate_setValueFromLong) {
		ObjectStoreItem testObj;

		long value = 12;
		long errorValue = 0;

		testObj.setValueFromLong(value);
		EXPECT_EQ(testObj.getValueAsLong(), value);
		EXPECT_EQ(testObj.getType(), ObjectDataItemStoreType::item_long);
	}

	TEST(ObjectStoreItem_Class, operate_setValueFromFloat) {
		ObjectStoreItem testObj;

		float value = 12.0;
		float errorValue = 0.0;

		testObj.setValueFromFloat(value);
		EXPECT_EQ(testObj.getValueAsFloat(), value);
		EXPECT_EQ(testObj.getType(), ObjectDataItemStoreType::item_float);
	}

	TEST(ObjectStoreItem_Class, operate_setValueFromDouble) {
		ObjectStoreItem testObj;

		double value = 12.0;
		double errorValue = 0.0;

		testObj.setValueFromDouble(value);
		EXPECT_EQ(testObj.getValueAsDouble(), value);
		EXPECT_EQ(testObj.getType(), ObjectDataItemStoreType::item_double);
	}

	TEST(ObjectStoreItem_Class, operate_setValueFromBool) {
		ObjectStoreItem testObj;

		bool value = true;
		bool errorValue = false;

		testObj.setValueFromBool(value);
		EXPECT_EQ(testObj.getValueAsBool(), value);
		EXPECT_EQ(testObj.getType(), ObjectDataItemStoreType::item_bool);
	}

	TEST(ObjectStoreItem_Class, opeatorEuqa_true) {
		ObjectStoreItem testObj;
		ObjectStoreItem testObj1;
		testObj.setName("testName");
		testObj.setValueFromBool(false);

		testObj1.setName("testName");
		testObj1.setValueFromBool(false);

		auto result = testObj == testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromDouble(1);
		testObj1.setValueFromDouble(1);
		result = testObj == testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromFloat(1);
		testObj1.setValueFromFloat(1);
		result = testObj == testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromInt(1);
		testObj1.setValueFromInt(1);
		result = testObj == testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromLong(1);
		testObj1.setValueFromLong(1);
		result = testObj == testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromString("test");
		testObj1.setValueFromString("test");
		result = testObj == testObj1;
		EXPECT_EQ(result, true);
	}

	TEST(ObjectStoreItem_Class, opeatorEuqa_false) {
		ObjectStoreItem testObj;
		ObjectStoreItem testObj1;
		testObj.setName("testName");
		testObj.setValueFromBool(false);
		testObj1.setName("testName");
		testObj1.setValueFromBool(true);
		auto result = testObj == testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromDouble(1);
		testObj1.setValueFromDouble(2);
		result = testObj == testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromFloat(1);
		testObj1.setValueFromFloat(2);
		result = testObj == testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromInt(1);
		testObj1.setValueFromInt(2);
		result = testObj == testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromLong(1);
		testObj1.setValueFromLong(2);
		result = testObj == testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromString("test");
		testObj1.setValueFromString("error");
		result = testObj == testObj1;
		EXPECT_EQ(result, false);
	}

	TEST(ObjectStoreItem_Class, opeatorNeEuqa_true) {
		ObjectStoreItem testObj;
		ObjectStoreItem testObj1;
		testObj.setName("testName");
		testObj.setValueFromBool(false);
		testObj1.setName("testName");
		testObj1.setValueFromBool(true);
		auto result = testObj != testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromDouble(1);
		testObj1.setValueFromDouble(2);
		result = testObj != testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromFloat(1);
		testObj1.setValueFromFloat(2);
		result = testObj != testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromInt(1);
		testObj1.setValueFromInt(2);
		result = testObj != testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromLong(1);
		testObj1.setValueFromLong(2);
		result = testObj != testObj1;
		EXPECT_EQ(result, true);

		testObj.setValueFromString("test");
		testObj1.setValueFromString("error");
		result = testObj != testObj1;
		EXPECT_EQ(result, true);
	}

	TEST(ObjectStoreItem_Class, opeatorNeEuqa_false) {
		ObjectStoreItem testObj;
		ObjectStoreItem testObj1;
		testObj.setName("testName");
		testObj.setValueFromBool(true);
		testObj1.setName("testName");
		testObj1.setValueFromBool(true);
		auto result = testObj != testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromDouble(1);
		testObj1.setValueFromDouble(1);
		result = testObj != testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromFloat(1);
		testObj1.setValueFromFloat(1);
		result = testObj != testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromInt(1);
		testObj1.setValueFromInt(1);
		result = testObj != testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromLong(1);
		testObj1.setValueFromLong(1);
		result = testObj != testObj1;
		EXPECT_EQ(result, false);

		testObj.setValueFromString("test");
		testObj1.setValueFromString("test");
		result = testObj != testObj1;
		EXPECT_EQ(result, false);
	}
}

namespace oso_core
{
	TEST(ObjectStoreAssembly_Class, CopyConstructor)
	{
		ObjectStoreAssembly testAssembly;
		testAssembly.setName("testName");
		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);
		ObjectStoreAssembly testAssembly1(testAssembly);
		EXPECT_EQ(testAssembly1.getName(), "testName");
		auto sourcePtr = testAssembly1.getItem("testItem");
		auto item = std::dynamic_pointer_cast<ObjectStoreItem>(sourcePtr);
		EXPECT_EQ(item->getName(), "testItem");
		EXPECT_EQ(item->getValueAsBool(), false);
		testItem->setName("error");
		EXPECT_EQ(item->getName(), "testItem");
	}

	TEST(ObjectStoreAssembly_Class, ConstructorMove)
	{
		ObjectStoreAssembly testAssembly;
		testAssembly.setName("testName");
		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);
		ObjectStoreAssembly testAssembly1(std::move(testAssembly));
		EXPECT_EQ(testAssembly1.getName(), "testName");
		auto sourcePtr = testAssembly1.getItem("testItem");
		auto item = std::dynamic_pointer_cast<ObjectStoreItem>(sourcePtr);
		EXPECT_EQ(item->getName(), "testItem");
		EXPECT_EQ(item->getValueAsBool(), false);
		testItem->setName("error");
		EXPECT_EQ(item->getName(), "testItem");
	}

	TEST(ObjectStoreAssembly_Class, operatorAssignMove)
	{
		ObjectStoreAssembly testAssembly;
		testAssembly.setName("testName");
		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);
		ObjectStoreAssembly testAssembly1;
		testAssembly1 = std::move(testAssembly);
		EXPECT_EQ(testAssembly1.getName(), "testName");
		auto sourcePtr = testAssembly1.getItem("testItem");
		auto item = std::dynamic_pointer_cast<ObjectStoreItem>(sourcePtr);
		EXPECT_EQ(item->getName(), "testItem");
		EXPECT_EQ(item->getValueAsBool(), false);
		testItem->setName("error");
		EXPECT_EQ(item->getName(), "testItem");
	}

	TEST(ObjectStoreAssembly_Class, AssignmentOperator)
	{
		ObjectStoreAssembly testAssembly;
		testAssembly.setName("testName");
		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);
		ObjectStoreAssembly testAssembly1;
		testAssembly1 = testAssembly;
		EXPECT_EQ(testAssembly1.getName(), "testName");
		auto item = std::dynamic_pointer_cast<ObjectStoreItem>(testAssembly1.getItem("testItem"));
		EXPECT_EQ(item->getName(), "testItem");
		EXPECT_EQ(item->getValueAsBool(), false);
		testItem->setName("error");
		EXPECT_EQ(item->getName(), "testItem");
	}

	TEST(ObjectStoreAssembly_Class, opeatorEuqa_true) {
		ObjectStoreAssembly testAssembly;
		ObjectStoreAssembly testAssembly1;

		testAssembly.setName("testName");
		testAssembly1.setName("testName");

		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);

		auto testItem1 = std::make_shared<ObjectStoreItem>();
		testItem1->setName("testItem");
		testItem1->setValueFromBool(false);
		testAssembly1.addItem(testItem1);

		auto result = testAssembly == testAssembly1;
		EXPECT_EQ(result, true);
	}

	TEST(ObjectStoreAssembly_Class, opeatorEuqa_false) {
		ObjectStoreAssembly testAssembly;
		ObjectStoreAssembly testAssembly1;

		testAssembly.setName("testName");
		testAssembly1.setName("testName");

		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);

		auto testItem1 = std::make_shared<ObjectStoreItem>();
		testItem1->setName("testItem");
		testItem1->setValueFromBool(true);
		testAssembly1.addItem(testItem1);

		auto result = testAssembly == testAssembly1;
		EXPECT_EQ(result, false);
	}

	TEST(ObjectStoreAssembly_Class, opeatorNeEuqa_true) {
		ObjectStoreAssembly testAssembly;
		ObjectStoreAssembly testAssembly1;

		testAssembly.setName("testName");
		testAssembly1.setName("testName");

		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);

		auto testItem1 = std::make_shared<ObjectStoreItem>();
		testItem1->setName("testItem");
		testItem1->setValueFromBool(true);
		testAssembly1.addItem(testItem1);

		auto result = testAssembly != testAssembly1;
		EXPECT_EQ(result, true);
	}

	TEST(ObjectStoreAssembly_Class, opeatorNeEuqa_false) {
		ObjectStoreAssembly testAssembly;
		ObjectStoreAssembly testAssembly1;

		testAssembly.setName("testName");
		testAssembly1.setName("testName");

		auto testItem = std::make_shared<ObjectStoreItem>();
		testItem->setName("testItem");
		testItem->setValueFromBool(false);
		testAssembly.addItem(testItem);

		auto testItem1 = std::make_shared<ObjectStoreItem>();
		testItem1->setName("testItem");
		testItem1->setValueFromBool(false);
		testAssembly1.addItem(testItem1);

		auto result = testAssembly != testAssembly1;
		EXPECT_EQ(result, false);
	}
}