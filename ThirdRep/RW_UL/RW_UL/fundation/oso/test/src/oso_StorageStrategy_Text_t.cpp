#include"oso_StorageStrategy_Text_t.hpp"

namespace oso_StorageStrategy_Text
{
	/*  TEST(a,a)
	  {
		  StorageStrategy_Text s;
		  ObjectStoreAssembly _sampleAssembly;

		  _sampleAssembly.setName("OcrDataXmlStoreFormatSampleData");

		  ObjectStoreItem boolItem;
		  boolItem.setName("bool");
		  boolItem.setValueFromBool(false);
		  _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(boolItem));

		  ObjectStoreItem doubleItem;
		  doubleItem.setName("double");
		  doubleItem.setValueFromDouble(1.1);
		  _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(doubleItem));

		  ObjectStoreItem intItem;
		  intItem.setName("int");
		  intItem.setValueFromInt(1);
		  _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(intItem));

		  ObjectStoreItem floatItem;
		  floatItem.setName("float");
		  floatItem.setValueFromFloat(1.1);
		  _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(floatItem));

		  ObjectStoreAssembly childAssembly;
		  childAssembly.setName("childAssembly");
		  ObjectStoreItem longItem;
		  longItem.setName("long");
		  longItem.setValueFromLong(1);
		  childAssembly.addItem(std::make_shared<ObjectStoreItem>(longItem));

		  ObjectStoreItem stringItem;
		  stringItem.setName("string");
		  stringItem.setValueFromString("string");
		  childAssembly.addItem(std::make_shared<ObjectStoreItem>(stringItem));

		  _sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));
		  _sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));
		  _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(boolItem));
		  _sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));

		  s.save(_sampleAssembly, R"(C:\Users\34615\Desktop\1\1.txt)");

		  auto str = s.getFormatString(_sampleAssembly);
		  std::cout << _sampleAssembly.getItems().size()<<std::endl;
		  std::cout << str.size();
	  }

	  TEST(a,b)
	  {
		  StorageStrategy_Text s;
		  s.load(R"(C:\Users\34615\Desktop\1\1.txt)");
	  }

	  TEST(a,c)
	  {
		  StorageStrategy_Text s;
		 auto a= s.load(R"(C:\Users\34615\Desktop\1\1.txt)");
		 auto str=s.getFormatString(*a.get());
		 std::cout << str.size();
	  }*/
}