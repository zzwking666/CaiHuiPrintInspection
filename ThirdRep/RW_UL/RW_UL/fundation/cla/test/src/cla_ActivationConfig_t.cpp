#include"cla_ActivationConfig_t.hpp"

namespace cla_ActivationCodeModule
{
	TEST(ActivationConfigStruct, serialize)
	{
		ActivationConfig config;
		config.insert("name1", "value1");
		config.insert("name2", "value2");
		config.insert("name3", "value3");
		config.insert("name4", "value4");
		config.insert("name5", "value5");
		config.insert("name6", "value6");
		config.insert("name7", "value7");
		config.insert("name7", "value7");

		auto str = ActivationConfig::serialize(config);
		ASSERT_EQ(str.empty(), false);
	}

	TEST(ActivationConfigStruct, deserialize)
	{
		ActivationConfig config;
		config.insert("name1", "value1");
		config.insert("name2", "value2");
		config.insert("name3", "value3");
		config.insert("name4", "value4");
		config.insert("name5", "value5");
		config.insert("name6", "value6");
		config.insert("name7", "value7");

		auto str = ActivationConfig::serialize(config);
		ASSERT_EQ(str.empty(), false);

		ActivationConfig deserializeConfig(str);
		ASSERT_EQ(ActivationConfig::serialize(deserializeConfig), str);
	}

	TEST(ActivationConfigStruct, ActivationConfigConstruct)
	{
		ActivationConfig config;
		config.insert("name1", "value1");
		config.insert("name2", "value2");
		config.insert("name3", "value3");
		config.insert("name4", "value4");
		config.insert("name5", "value5");
		config.insert("name6", "value6");
		config.insert("name7", "value7");

		auto str = ActivationConfig::serialize(config);
		ASSERT_EQ(str.empty(), false);

		ActivationConfig deserializeConfig(config);
		ASSERT_EQ(ActivationConfig::serialize(deserializeConfig), str);

		ActivationConfig config1;
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
}