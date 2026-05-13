#pragma once

#include"oso_core.h"
#include <string>

namespace cdm {
    class GeneralConfig
    {
    public:
        GeneralConfig() = default;
        ~GeneralConfig() = default;

        GeneralConfig(const rw::oso::ObjectStoreAssembly& assembly);
        GeneralConfig(const GeneralConfig& obj);

        GeneralConfig& operator=(const GeneralConfig& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const GeneralConfig& obj) const;
        bool operator!=(const GeneralConfig& obj) const;

    public:
        bool iszhinengcaiqie{ false };
        int shengchanzongliang{ 0 };
        double shengchanlianglv{ 0 };
        int feipinshuliang{ 0 };
        double pingjundaichang{ 0 };
        bool istifei{ false };
        bool isyinshuajiance{ false };
        double liangdu{ 0 };
        int daizizhonglei{ 0 };
        double baoguang{ 0 };
        double test{ 0 };
        bool test1{ false };
        int test2{ 0 };
        long test3{ 0 };
    };

    inline GeneralConfig::GeneralConfig(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$GeneralConfig$")
        {
            throw std::runtime_error("Assembly is not $class$GeneralConfig$");
        }
        auto iszhinengcaiqieItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$iszhinengcaiqie$"));
        if (!iszhinengcaiqieItem) {
            throw std::runtime_error("$variable$iszhinengcaiqie is not found");
        }
        iszhinengcaiqie = iszhinengcaiqieItem->getValueAsBool();
        auto shengchanzongliangItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shengchanzongliang$"));
        if (!shengchanzongliangItem) {
            throw std::runtime_error("$variable$shengchanzongliang is not found");
        }
        shengchanzongliang = shengchanzongliangItem->getValueAsInt();
        auto shengchanlianglvItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shengchanlianglv$"));
        if (!shengchanlianglvItem) {
            throw std::runtime_error("$variable$shengchanlianglv is not found");
        }
        shengchanlianglv = shengchanlianglvItem->getValueAsDouble();
        auto feipinshuliangItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$feipinshuliang$"));
        if (!feipinshuliangItem) {
            throw std::runtime_error("$variable$feipinshuliang is not found");
        }
        feipinshuliang = feipinshuliangItem->getValueAsInt();
        auto pingjundaichangItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$pingjundaichang$"));
        if (!pingjundaichangItem) {
            throw std::runtime_error("$variable$pingjundaichang is not found");
        }
        pingjundaichang = pingjundaichangItem->getValueAsDouble();
        auto istifeiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$istifei$"));
        if (!istifeiItem) {
            throw std::runtime_error("$variable$istifei is not found");
        }
        istifei = istifeiItem->getValueAsBool();
        auto isyinshuajianceItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$isyinshuajiance$"));
        if (!isyinshuajianceItem) {
            throw std::runtime_error("$variable$isyinshuajiance is not found");
        }
        isyinshuajiance = isyinshuajianceItem->getValueAsBool();
        auto liangduItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$liangdu$"));
        if (!liangduItem) {
            throw std::runtime_error("$variable$liangdu is not found");
        }
        liangdu = liangduItem->getValueAsDouble();
        auto daizizhongleiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$daizizhonglei$"));
        if (!daizizhongleiItem) {
            throw std::runtime_error("$variable$daizizhonglei is not found");
        }
        daizizhonglei = daizizhongleiItem->getValueAsInt();
        auto baoguangItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$baoguang$"));
        if (!baoguangItem) {
            throw std::runtime_error("$variable$baoguang is not found");
        }
        baoguang = baoguangItem->getValueAsDouble();
        auto testItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test$"));
        if (!testItem) {
            throw std::runtime_error("$variable$test is not found");
        }
        test = testItem->getValueAsDouble();
        auto test1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test1$"));
        if (!test1Item) {
            throw std::runtime_error("$variable$test1 is not found");
        }
        test1 = test1Item->getValueAsBool();
        auto test2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test2$"));
        if (!test2Item) {
            throw std::runtime_error("$variable$test2 is not found");
        }
        test2 = test2Item->getValueAsInt();
        auto test3Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$test3$"));
        if (!test3Item) {
            throw std::runtime_error("$variable$test3 is not found");
        }
        test3 = test3Item->getValueAsLong();
    }

    inline GeneralConfig::GeneralConfig(const GeneralConfig& obj)
    {
        iszhinengcaiqie = obj.iszhinengcaiqie;
        shengchanzongliang = obj.shengchanzongliang;
        shengchanlianglv = obj.shengchanlianglv;
        feipinshuliang = obj.feipinshuliang;
        pingjundaichang = obj.pingjundaichang;
        istifei = obj.istifei;
        isyinshuajiance = obj.isyinshuajiance;
        liangdu = obj.liangdu;
        daizizhonglei = obj.daizizhonglei;
        baoguang = obj.baoguang;
        test = obj.test;
        test1 = obj.test1;
        test2 = obj.test2;
        test3 = obj.test3;
    }

    inline GeneralConfig& GeneralConfig::operator=(const GeneralConfig& obj)
    {
        if (this != &obj) {
            iszhinengcaiqie = obj.iszhinengcaiqie;
            shengchanzongliang = obj.shengchanzongliang;
            shengchanlianglv = obj.shengchanlianglv;
            feipinshuliang = obj.feipinshuliang;
            pingjundaichang = obj.pingjundaichang;
            istifei = obj.istifei;
            isyinshuajiance = obj.isyinshuajiance;
            liangdu = obj.liangdu;
            daizizhonglei = obj.daizizhonglei;
            baoguang = obj.baoguang;
            test = obj.test;
            test1 = obj.test1;
            test2 = obj.test2;
            test3 = obj.test3;
        }
        return *this;
    }

    inline GeneralConfig::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$GeneralConfig$");
        auto iszhinengcaiqieItem = std::make_shared<rw::oso::ObjectStoreItem>();
        iszhinengcaiqieItem->setName("$variable$iszhinengcaiqie$");
        iszhinengcaiqieItem->setValueFromBool(iszhinengcaiqie);
        assembly.addItem(iszhinengcaiqieItem);
        auto shengchanzongliangItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shengchanzongliangItem->setName("$variable$shengchanzongliang$");
        shengchanzongliangItem->setValueFromInt(shengchanzongliang);
        assembly.addItem(shengchanzongliangItem);
        auto shengchanlianglvItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shengchanlianglvItem->setName("$variable$shengchanlianglv$");
        shengchanlianglvItem->setValueFromDouble(shengchanlianglv);
        assembly.addItem(shengchanlianglvItem);
        auto feipinshuliangItem = std::make_shared<rw::oso::ObjectStoreItem>();
        feipinshuliangItem->setName("$variable$feipinshuliang$");
        feipinshuliangItem->setValueFromInt(feipinshuliang);
        assembly.addItem(feipinshuliangItem);
        auto pingjundaichangItem = std::make_shared<rw::oso::ObjectStoreItem>();
        pingjundaichangItem->setName("$variable$pingjundaichang$");
        pingjundaichangItem->setValueFromDouble(pingjundaichang);
        assembly.addItem(pingjundaichangItem);
        auto istifeiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        istifeiItem->setName("$variable$istifei$");
        istifeiItem->setValueFromBool(istifei);
        assembly.addItem(istifeiItem);
        auto isyinshuajianceItem = std::make_shared<rw::oso::ObjectStoreItem>();
        isyinshuajianceItem->setName("$variable$isyinshuajiance$");
        isyinshuajianceItem->setValueFromBool(isyinshuajiance);
        assembly.addItem(isyinshuajianceItem);
        auto liangduItem = std::make_shared<rw::oso::ObjectStoreItem>();
        liangduItem->setName("$variable$liangdu$");
        liangduItem->setValueFromDouble(liangdu);
        assembly.addItem(liangduItem);
        auto daizizhongleiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        daizizhongleiItem->setName("$variable$daizizhonglei$");
        daizizhongleiItem->setValueFromInt(daizizhonglei);
        assembly.addItem(daizizhongleiItem);
        auto baoguangItem = std::make_shared<rw::oso::ObjectStoreItem>();
        baoguangItem->setName("$variable$baoguang$");
        baoguangItem->setValueFromDouble(baoguang);
        assembly.addItem(baoguangItem);
        auto testItem = std::make_shared<rw::oso::ObjectStoreItem>();
        testItem->setName("$variable$test$");
        testItem->setValueFromDouble(test);
        assembly.addItem(testItem);
        auto test1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test1Item->setName("$variable$test1$");
        test1Item->setValueFromBool(test1);
        assembly.addItem(test1Item);
        auto test2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test2Item->setName("$variable$test2$");
        test2Item->setValueFromInt(test2);
        assembly.addItem(test2Item);
        auto test3Item = std::make_shared<rw::oso::ObjectStoreItem>();
        test3Item->setName("$variable$test3$");
        test3Item->setValueFromLong(test3);
        assembly.addItem(test3Item);
        return assembly;
    }

    inline bool GeneralConfig::operator==(const GeneralConfig& obj) const
    {
        return iszhinengcaiqie == obj.iszhinengcaiqie && shengchanzongliang == obj.shengchanzongliang && shengchanlianglv == obj.shengchanlianglv && feipinshuliang == obj.feipinshuliang && pingjundaichang == obj.pingjundaichang && istifei == obj.istifei && isyinshuajiance == obj.isyinshuajiance && liangdu == obj.liangdu && daizizhonglei == obj.daizizhonglei && baoguang == obj.baoguang && test == obj.test && test1 == obj.test1 && test2 == obj.test2 && test3 == obj.test3;
    }

    inline bool GeneralConfig::operator!=(const GeneralConfig& obj) const
    {
        return !(*this == obj);
    }

}

