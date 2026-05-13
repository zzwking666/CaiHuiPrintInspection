# OSO(Object Storage Organization)

## 介绍
用于存储对象的组织方式，主要包括对象的命名规范、对象的存储位置、对象的元数据等。

## 封装好的对象

- FileSave
- ObjectStoreItem
- ObjectStoreAssembly

## 快速开始

### 存储一个对象
```cpp

int main()
{
    ObjectStoreAssembly car;
    car.setName("Car");

    ObjectStoreItem winodw;
    winodw.setName("window");
    car.addItem(std::move(winodw));

    ObjectStoreItem door;
    door.setName("door");
    car.addItem(std::move(door));

    ObjectStoreItem wheel;
    wheel.setName("wheel");
    car.addItem(std::move(wheel));

    ObjectStoreAssembly engine;
    engine.setName("engine");

    ObjectStoreItem engineType;
    engineType.setName("engineType");
    engine.addItem(std::move(engineType));


    ObjectStoreItem engineSize;
    engineSize.setName("engineSize");
    engine.addItem(std::move(engineSize));

    car.addItem(std::move(engine));



    FileSave<FileSaveStrategyType::XML> fileSave;
    fileSave.save(R"(你要存储文件的绝对路径)", std::make_shared<ObjectStoreAssembly>(car));

    return 0;

}


```
那么在xml文件中会生成如下内容
```xml
<?xml version="1.0"?>
<Car nodeType="root">
	<window type="int" nodeType="item">
		<value>0</value>
	</window>
	<door type="int" nodeType="item">
		<value>0</value>
	</door>
	<wheel type="int" nodeType="item">
		<value>0</value>
	</wheel>
	<engine nodeType="assembly">
		<engineType type="int" nodeType="item">
			<value>0</value>
		</engineType>
		<engineSize type="int" nodeType="item">
			<value>0</value>
		</engineSize>
	</engine>
</Car>
```

### 读取一个对象并打印在控制台

```cpp
#include"oso/oso_core.h"
#include"oso/osos_FileSave.h"

#include<iostream>

using namespace rw::oso;

int main()
{
	FileSave<FileSaveStrategyType::XML> fileSave;
	auto car = fileSave.load(R"(你要读取文件的绝对路径)");
    car->print(std::cout);
	return 0;
}
```

那么在控制台会打印如下内容
```
-Car
  *window: 0 (int)
  *door: 0 (int)
  *wheel: 0 (int)
  -engine
    *engineType: 0 (int)
    *engineSize: 0 (int)
```

### 存储已经支持的类型（后期用户需要存储自定义类型需要自己写转换函数）
```cpp
#include"oso/oso_core.h"
#include"oso/osos_FileSave.h"
#include"oso/osop_OcrDateProductConfig.h"

#include<iostream>

using namespace rw::oso;

int main()
{
    OcrDataProductConfig config;
    config.productName = "Config1";
	FileSave<FileSaveStrategyType::XML> fileSave;
    fileSave.save(R"(你要存储文件的绝对路径)", makeObjectStoreAssemblySharedPtr(OcrDataProductConfig::toObjectStoreAssembly(config)));

	return 0;
}

```

那么在xml文件中会生成如下内容
```xml
<?xml version="1.0"?>
<OcrDataProductConfig nodeType="root">
	<productName type="string" nodeType="item">
		<value>Config1</value>
	</productName>
	<cameraAttributesBasic nodeType="assembly">
		<exposureTime type="long" nodeType="item">
			<value>10000</value>
		</exposureTime>
		<gain type="int" nodeType="item">
			<value>1</value>
		</gain>
	</cameraAttributesBasic>
	<recognizeRange nodeType="assembly">
		<leftTopCorner_x type="double" nodeType="item">
			<value>0.000000</value>
		</leftTopCorner_x>
		<leftTopCorner_y type="double" nodeType="item">
			<value>0.000000</value>
		</leftTopCorner_y>
		<leftLowerCorner_x type="double" nodeType="item">
			<value>0.000000</value>
		</leftLowerCorner_x>
		<leftLowerCorner_y type="double" nodeType="item">
			<value>0.000000</value>
		</leftLowerCorner_y>
		<rightTopCorner_x type="double" nodeType="item">
			<value>0.000000</value>
		</rightTopCorner_x>
		<rightTopCorner_y type="double" nodeType="item">
			<value>0.000000</value>
		</rightTopCorner_y>
		<rightLowerCorner_x type="double" nodeType="item">
			<value>0.000000</value>
		</rightLowerCorner_x>
		<rightLowerCorner_y type="double" nodeType="item">
			<value>0.000000</value>
		</rightLowerCorner_y>
	</recognizeRange>
	<productCheckCount nodeType="assembly">
		<totalCount type="int" nodeType="item">
			<value>0</value>
		</totalCount>
		<passCount type="int" nodeType="item">
			<value>0</value>
		</passCount>
		<outCount type="int" nodeType="item">
			<value>0</value>
		</outCount>
	</productCheckCount>
	<rejectAttribute nodeType="assembly">
		<RejectDelay type="int" nodeType="item">
			<value>0</value>
		</RejectDelay>
		<OffsetsNumber type="int" nodeType="item">
			<value>0</value>
		</OffsetsNumber>
		<DisposalTime type="int" nodeType="item">
			<value>0</value>
		</DisposalTime>
	</rejectAttribute>
</OcrDataProductConfig>

```



