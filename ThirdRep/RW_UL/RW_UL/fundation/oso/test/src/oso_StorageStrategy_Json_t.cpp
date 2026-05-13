#include"oso_StorageStrategy_Json_t.hpp"

//TEST(a,a)
//{
//    StorageStrategy_Json s;
//    ObjectStoreAssembly _sampleAssembly;
//
//    ObjectStoreItem boolItem;
//    boolItem.setName("bool");
//    boolItem.setValueFromBool(false);
//    _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(boolItem));
//
//    ObjectStoreItem doubleItem;
//    doubleItem.setName("double");
//    doubleItem.setValueFromDouble(1.1);
//    _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(doubleItem));
//
//    ObjectStoreItem intItem;
//    intItem.setName("int");
//    intItem.setValueFromInt(1);
//    _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(intItem));
//
//    ObjectStoreItem floatItem;
//    floatItem.setName("float");
//    floatItem.setValueFromFloat(1.1);
//    _sampleAssembly.addItem(std::make_shared<ObjectStoreItem>(floatItem));
//
//
//    ObjectStoreAssembly childAssembly;
//    childAssembly.setName("childAssembly");
//    ObjectStoreItem longItem;
//    longItem.setName("long");
//    longItem.setValueFromLong(1);
//    childAssembly.addItem(std::make_shared<ObjectStoreItem>(longItem));
//
//    ObjectStoreItem stringItem;
//    stringItem.setName("string");
//    stringItem.setValueFromString("string");
//    childAssembly.addItem(std::make_shared<ObjectStoreItem>(stringItem));
//
//    _sampleAssembly.addItem(std::make_shared<ObjectStoreAssembly>(childAssembly));
//
//    s.save(_sampleAssembly, R"(C:\Users\34615\Desktop\1\1.json)");
//
//}