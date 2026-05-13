#include"cla_StrEncryption_t.hpp"

namespace cla_StrEncryption
{
	TEST(StrConfigStruct, serialize)
	{
		StrConfig config;
		config.insert("name1", "value1");
		config.insert("name2", "value2");
		config.insert("name3", "value3");
		config.insert("name4", "value4");
		config.insert("name5", "value5");
		config.insert("name6", "value6");
		config.insert("name7", "value7");
		config.insert("name7", "value7");

		auto str = StrConfig::serialize(config);
		ASSERT_EQ(str.empty(), false);
	}

	TEST(StrConfigStruct, deserialize)
	{
		StrConfig config;
		config.insert("name1", "value1");
		config.insert("name2", "value2");
		config.insert("name3", "value3");
		config.insert("name4", "value4");
		config.insert("name5", "value5");
		config.insert("name6", "value6");
		config.insert("name7", "value7");

		auto str = StrConfig::serialize(config);
		ASSERT_EQ(str.empty(), false);

		StrConfig deserializeConfig(str);
		ASSERT_EQ(StrConfig::serialize(deserializeConfig), str);
	}

	TEST(StrConfigStruct, ActivationConfigConstruct)
	{
		StrConfig config;
		config.insert("name1", "value1");
		config.insert("name2", "value2");
		config.insert("name3", "value3");
		config.insert("name4", "value4");
		config.insert("name5", "value5");
		config.insert("name6", "value6");
		config.insert("name7", "value7");

		auto str = StrConfig::serialize(config);
		ASSERT_EQ(str.empty(), false);

		StrConfig deserializeConfig(config);
		ASSERT_EQ(StrConfig::serialize(deserializeConfig), str);

		StrConfig config1;
		config1.insert("name1", "value1");
		config1.insert("name2", "value2");
		config1.insert("name3", "value3");
		config1.insert("name4", "value4");
		config1.insert("name5", "value5");
		config1.insert("name6", "value6");
		ASSERT_NE(config1, config);

		ASSERT_EQ(config1 == config, false);

		config1.insert("name7", "value7");
		ASSERT_EQ(config1, config);

		ASSERT_EQ(config1 == config, true);
	}

	TEST(StrEncryptionClass, StrEncryptionConstruct)
	{
		StrConfig config;
		config.insert("name1", "value1");


		auto str=StrEncryption::StrConfigToHex(config, "test");
		bool isOK = false;
		auto decryptConfig = StrEncryption::StrConfigFromHex(str, "test", isOK);
		ASSERT_EQ(isOK, true);
	}
}