#pragma once

#include"oso_core.h"
#include <string>
#include <vector>

namespace cdm {
    class testClass
    {
    public:
        testClass() = default;
        ~testClass() = default;

        testClass(const rw::oso::ObjectStoreAssembly& assembly);
        testClass(const testClass& obj);

        testClass& operator=(const testClass& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const testClass& obj) const;
        bool operator!=(const testClass& obj) const;

    public:
        size_t expousureTime{};
        size_t test1{ 1000 };
        int test2{ 1000 };
        std::string test3{ "1000" };
        bool test4{ true };
    };

    inline testClass::testClass(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$testClass$")
        {
            throw std::runtime_error("Assembly is not $class$testClass$");
        }
        auto expousureTimeItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$expousureTime$"));
        if (!expousureTimeItem) {
            throw std::runtime_error("$variable$expousureTime is not found");
        }
        expousureTime = expousureTimeItem->getValueAsLong();
        auto test1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test1$"));
        if (!test1Item) {
            throw std::runtime_error("$variable$test1 is not found");
        }
        test1 = test1Item->getValueAsLong();
        auto test2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test2$"));
        if (!test2Item) {
            throw std::runtime_error("$variable$test2 is not found");
        }
        test2 = test2Item->getValueAsInt();
        auto test3Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test3$"));
        if (!test3Item) {
            throw std::runtime_error("$variable$test3 is not found");
        }
        test3 = test3Item->getValueAsString();
        auto test4Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test4$"));
        if (!test4Item) {
            throw std::runtime_error("$variable$test4 is not found");
        }
        test4 = test4Item->getValueAsBool();
    }

    inline testClass::testClass(const testClass& obj)
    {
        expousureTime = obj.expousureTime;
        test1 = obj.test1;
        test2 = obj.test2;
        test3 = obj.test3;
        test4 = obj.test4;
    }

    inline testClass& testClass::operator=(const testClass& obj)
    {
        if (this != &obj) {
            expousureTime = obj.expousureTime;
            test1 = obj.test1;
            test2 = obj.test2;
            test3 = obj.test3;
            test4 = obj.test4;
        }
        return *this;
    }

    inline testClass::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$testClass$");
        auto expousureTimeItem = std::make_shared<rw::oso::ObjectStoreItem>();
        expousureTimeItem->setName("$variable$expousureTime$");
        expousureTimeItem->setValueFromLong(expousureTime);
        assembly.addItem(expousureTimeItem);
        auto test1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test1Item->setName("$variable$test1$");
        test1Item->setValueFromLong(test1);
        assembly.addItem(test1Item);
        auto test2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test2Item->setName("$variable$test2$");
        test2Item->setValueFromInt(test2);
        assembly.addItem(test2Item);
        auto test3Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test3Item->setName("$variable$test3$");
        test3Item->setValueFromString(test3);
        assembly.addItem(test3Item);
        auto test4Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test4Item->setName("$variable$test4$");
        test4Item->setValueFromBool(test4);
        assembly.addItem(test4Item);
        return assembly;
    }

    inline bool testClass::operator==(const testClass& obj) const
    {
        return expousureTime == obj.expousureTime && test1 == obj.test1 && test2 == obj.test2 && test3 == obj.test3 && test4 == obj.test4;
    }

    inline bool testClass::operator!=(const testClass& obj) const
    {
        return !(*this == obj);
    }

    class testClass1
    {
    public:
        testClass1() = default;
        ~testClass1() = default;

        testClass1(const rw::oso::ObjectStoreAssembly& assembly);
        testClass1(const testClass1& obj);

        testClass1& operator=(const testClass1& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const testClass1& obj) const;
        bool operator!=(const testClass1& obj) const;

    public:
        size_t expousureTime{};
        size_t test1{ 1000 };
        int test2{ 1000 };
        std::string test3{ "1000" };
        bool test4{ true };
    };

    inline testClass1::testClass1(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$testClass1$")
        {
            throw std::runtime_error("Assembly is not $class$testClass1$");
        }
        auto expousureTimeItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$expousureTime$"));
        if (!expousureTimeItem) {
            throw std::runtime_error("$variable$expousureTime is not found");
        }
        expousureTime = expousureTimeItem->getValueAsLong();
        auto test1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test1$"));
        if (!test1Item) {
            throw std::runtime_error("$variable$test1 is not found");
        }
        test1 = test1Item->getValueAsLong();
        auto test2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test2$"));
        if (!test2Item) {
            throw std::runtime_error("$variable$test2 is not found");
        }
        test2 = test2Item->getValueAsInt();
        auto test3Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test3$"));
        if (!test3Item) {
            throw std::runtime_error("$variable$test3 is not found");
        }
        test3 = test3Item->getValueAsString();
        auto test4Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test4$"));
        if (!test4Item) {
            throw std::runtime_error("$variable$test4 is not found");
        }
        test4 = test4Item->getValueAsBool();
    }

    inline testClass1::testClass1(const testClass1& obj)
    {
        expousureTime = obj.expousureTime;
        test1 = obj.test1;
        test2 = obj.test2;
        test3 = obj.test3;
        test4 = obj.test4;
    }

    inline testClass1& testClass1::operator=(const testClass1& obj)
    {
        if (this != &obj) {
            expousureTime = obj.expousureTime;
            test1 = obj.test1;
            test2 = obj.test2;
            test3 = obj.test3;
            test4 = obj.test4;
        }
        return *this;
    }

    inline testClass1::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$testClass1$");
        auto expousureTimeItem = std::make_shared<rw::oso::ObjectStoreItem>();
        expousureTimeItem->setName("$variable$expousureTime$");
        expousureTimeItem->setValueFromLong(expousureTime);
        assembly.addItem(expousureTimeItem);
        auto test1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test1Item->setName("$variable$test1$");
        test1Item->setValueFromLong(test1);
        assembly.addItem(test1Item);
        auto test2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test2Item->setName("$variable$test2$");
        test2Item->setValueFromInt(test2);
        assembly.addItem(test2Item);
        auto test3Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test3Item->setName("$variable$test3$");
        test3Item->setValueFromString(test3);
        assembly.addItem(test3Item);
        auto test4Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test4Item->setName("$variable$test4$");
        test4Item->setValueFromBool(test4);
        assembly.addItem(test4Item);
        return assembly;
    }

    inline bool testClass1::operator==(const testClass1& obj) const
    {
        return expousureTime == obj.expousureTime && test1 == obj.test1 && test2 == obj.test2 && test3 == obj.test3 && test4 == obj.test4;
    }

    inline bool testClass1::operator!=(const testClass1& obj) const
    {
        return !(*this == obj);
    }

}

