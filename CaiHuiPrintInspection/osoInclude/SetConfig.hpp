#pragma once

#include"oso_core.h"
#include <string>

namespace cdm {
    class SetConfig
    {
    public:
        SetConfig() = default;
        ~SetConfig() = default;

        SetConfig(const rw::oso::ObjectStoreAssembly& assembly);
        SetConfig(const SetConfig& obj);

        SetConfig& operator=(const SetConfig& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const SetConfig& obj) const;
        bool operator!=(const SetConfig& obj) const;

    public:
        double xiangjiguangdianpingbishijian{ 0 };
        double shuchuxinhaoyanshi{ 0 };
        double shuchuxinhaochixushijian{ 0 };
        double shangxianwei1{ 0 };
        double xiaxianwei1{ 0 };
        double zuoxianwei1{ 0 };
        double youxianwei1{ 0 };
        double baoguang1{ 0 };
        double zengyi1{ 0 };
        double xiangsudangliang1{ 0 };
        double shangxianwei2{ 0 };
        double xiaxianwei2{ 0 };
        double zuoxianwei2{ 0 };
        double youxianwei2{ 0 };
        double baoguang2{ 0 };
        double zengyi2{ 0 };
        double xiangsudangliang2{ 0 };
        int changeLanguageIndex{ 0 };
    };

    inline SetConfig::SetConfig(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$SetConfig$")
        {
            throw std::runtime_error("Assembly is not $class$SetConfig$");
        }
        auto xiangjiguangdianpingbishijianItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangjiguangdianpingbishijian$"));
        if (!xiangjiguangdianpingbishijianItem) {
            throw std::runtime_error("$variable$xiangjiguangdianpingbishijian is not found");
        }
        xiangjiguangdianpingbishijian = xiangjiguangdianpingbishijianItem->getValueAsDouble();
        auto shuchuxinhaoyanshiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shuchuxinhaoyanshi$"));
        if (!shuchuxinhaoyanshiItem) {
            throw std::runtime_error("$variable$shuchuxinhaoyanshi is not found");
        }
        shuchuxinhaoyanshi = shuchuxinhaoyanshiItem->getValueAsDouble();
        auto shuchuxinhaochixushijianItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shuchuxinhaochixushijian$"));
        if (!shuchuxinhaochixushijianItem) {
            throw std::runtime_error("$variable$shuchuxinhaochixushijian is not found");
        }
        shuchuxinhaochixushijian = shuchuxinhaochixushijianItem->getValueAsDouble();
        auto shangxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shangxianwei1$"));
        if (!shangxianwei1Item) {
            throw std::runtime_error("$variable$shangxianwei1 is not found");
        }
        shangxianwei1 = shangxianwei1Item->getValueAsDouble();
        auto xiaxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiaxianwei1$"));
        if (!xiaxianwei1Item) {
            throw std::runtime_error("$variable$xiaxianwei1 is not found");
        }
        xiaxianwei1 = xiaxianwei1Item->getValueAsDouble();
        auto zuoxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zuoxianwei1$"));
        if (!zuoxianwei1Item) {
            throw std::runtime_error("$variable$zuoxianwei1 is not found");
        }
        zuoxianwei1 = zuoxianwei1Item->getValueAsDouble();
        auto youxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$youxianwei1$"));
        if (!youxianwei1Item) {
            throw std::runtime_error("$variable$youxianwei1 is not found");
        }
        youxianwei1 = youxianwei1Item->getValueAsDouble();
        auto baoguang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$baoguang1$"));
        if (!baoguang1Item) {
            throw std::runtime_error("$variable$baoguang1 is not found");
        }
        baoguang1 = baoguang1Item->getValueAsDouble();
        auto zengyi1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zengyi1$"));
        if (!zengyi1Item) {
            throw std::runtime_error("$variable$zengyi1 is not found");
        }
        zengyi1 = zengyi1Item->getValueAsDouble();
        auto xiangsudangliang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangsudangliang1$"));
        if (!xiangsudangliang1Item) {
            throw std::runtime_error("$variable$xiangsudangliang1 is not found");
        }
        xiangsudangliang1 = xiangsudangliang1Item->getValueAsDouble();
        auto shangxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shangxianwei2$"));
        if (!shangxianwei2Item) {
            throw std::runtime_error("$variable$shangxianwei2 is not found");
        }
        shangxianwei2 = shangxianwei2Item->getValueAsDouble();
        auto xiaxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiaxianwei2$"));
        if (!xiaxianwei2Item) {
            throw std::runtime_error("$variable$xiaxianwei2 is not found");
        }
        xiaxianwei2 = xiaxianwei2Item->getValueAsDouble();
        auto zuoxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zuoxianwei2$"));
        if (!zuoxianwei2Item) {
            throw std::runtime_error("$variable$zuoxianwei2 is not found");
        }
        zuoxianwei2 = zuoxianwei2Item->getValueAsDouble();
        auto youxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$youxianwei2$"));
        if (!youxianwei2Item) {
            throw std::runtime_error("$variable$youxianwei2 is not found");
        }
        youxianwei2 = youxianwei2Item->getValueAsDouble();
        auto baoguang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$baoguang2$"));
        if (!baoguang2Item) {
            throw std::runtime_error("$variable$baoguang2 is not found");
        }
        baoguang2 = baoguang2Item->getValueAsDouble();
        auto zengyi2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zengyi2$"));
        if (!zengyi2Item) {
            throw std::runtime_error("$variable$zengyi2 is not found");
        }
        zengyi2 = zengyi2Item->getValueAsDouble();
        auto xiangsudangliang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangsudangliang2$"));
        if (!xiangsudangliang2Item) {
            throw std::runtime_error("$variable$xiangsudangliang2 is not found");
        }
        xiangsudangliang2 = xiangsudangliang2Item->getValueAsDouble();
        auto changeLanguageIndexItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$changeLanguageIndex$"));
        if (!changeLanguageIndexItem) {
            throw std::runtime_error("$variable$changeLanguageIndex is not found");
        }
        changeLanguageIndex = changeLanguageIndexItem->getValueAsInt();
    }

    inline SetConfig::SetConfig(const SetConfig& obj)
    {
        xiangjiguangdianpingbishijian = obj.xiangjiguangdianpingbishijian;
        shuchuxinhaoyanshi = obj.shuchuxinhaoyanshi;
        shuchuxinhaochixushijian = obj.shuchuxinhaochixushijian;
        shangxianwei1 = obj.shangxianwei1;
        xiaxianwei1 = obj.xiaxianwei1;
        zuoxianwei1 = obj.zuoxianwei1;
        youxianwei1 = obj.youxianwei1;
        baoguang1 = obj.baoguang1;
        zengyi1 = obj.zengyi1;
        xiangsudangliang1 = obj.xiangsudangliang1;
        shangxianwei2 = obj.shangxianwei2;
        xiaxianwei2 = obj.xiaxianwei2;
        zuoxianwei2 = obj.zuoxianwei2;
        youxianwei2 = obj.youxianwei2;
        baoguang2 = obj.baoguang2;
        zengyi2 = obj.zengyi2;
        xiangsudangliang2 = obj.xiangsudangliang2;
        changeLanguageIndex = obj.changeLanguageIndex;
    }

    inline SetConfig& SetConfig::operator=(const SetConfig& obj)
    {
        if (this != &obj) {
            xiangjiguangdianpingbishijian = obj.xiangjiguangdianpingbishijian;
            shuchuxinhaoyanshi = obj.shuchuxinhaoyanshi;
            shuchuxinhaochixushijian = obj.shuchuxinhaochixushijian;
            shangxianwei1 = obj.shangxianwei1;
            xiaxianwei1 = obj.xiaxianwei1;
            zuoxianwei1 = obj.zuoxianwei1;
            youxianwei1 = obj.youxianwei1;
            baoguang1 = obj.baoguang1;
            zengyi1 = obj.zengyi1;
            xiangsudangliang1 = obj.xiangsudangliang1;
            shangxianwei2 = obj.shangxianwei2;
            xiaxianwei2 = obj.xiaxianwei2;
            zuoxianwei2 = obj.zuoxianwei2;
            youxianwei2 = obj.youxianwei2;
            baoguang2 = obj.baoguang2;
            zengyi2 = obj.zengyi2;
            xiangsudangliang2 = obj.xiangsudangliang2;
            changeLanguageIndex = obj.changeLanguageIndex;
        }
        return *this;
    }

    inline SetConfig::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$SetConfig$");
        auto xiangjiguangdianpingbishijianItem = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangjiguangdianpingbishijianItem->setName("$variable$xiangjiguangdianpingbishijian$");
        xiangjiguangdianpingbishijianItem->setValueFromDouble(xiangjiguangdianpingbishijian);
        assembly.addItem(xiangjiguangdianpingbishijianItem);
        auto shuchuxinhaoyanshiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shuchuxinhaoyanshiItem->setName("$variable$shuchuxinhaoyanshi$");
        shuchuxinhaoyanshiItem->setValueFromDouble(shuchuxinhaoyanshi);
        assembly.addItem(shuchuxinhaoyanshiItem);
        auto shuchuxinhaochixushijianItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shuchuxinhaochixushijianItem->setName("$variable$shuchuxinhaochixushijian$");
        shuchuxinhaochixushijianItem->setValueFromDouble(shuchuxinhaochixushijian);
        assembly.addItem(shuchuxinhaochixushijianItem);
        auto shangxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shangxianwei1Item->setName("$variable$shangxianwei1$");
        shangxianwei1Item->setValueFromDouble(shangxianwei1);
        assembly.addItem(shangxianwei1Item);
        auto xiaxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiaxianwei1Item->setName("$variable$xiaxianwei1$");
        xiaxianwei1Item->setValueFromDouble(xiaxianwei1);
        assembly.addItem(xiaxianwei1Item);
        auto zuoxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zuoxianwei1Item->setName("$variable$zuoxianwei1$");
        zuoxianwei1Item->setValueFromDouble(zuoxianwei1);
        assembly.addItem(zuoxianwei1Item);
        auto youxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        youxianwei1Item->setName("$variable$youxianwei1$");
        youxianwei1Item->setValueFromDouble(youxianwei1);
        assembly.addItem(youxianwei1Item);
        auto baoguang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        baoguang1Item->setName("$variable$baoguang1$");
        baoguang1Item->setValueFromDouble(baoguang1);
        assembly.addItem(baoguang1Item);
        auto zengyi1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zengyi1Item->setName("$variable$zengyi1$");
        zengyi1Item->setValueFromDouble(zengyi1);
        assembly.addItem(zengyi1Item);
        auto xiangsudangliang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangsudangliang1Item->setName("$variable$xiangsudangliang1$");
        xiangsudangliang1Item->setValueFromDouble(xiangsudangliang1);
        assembly.addItem(xiangsudangliang1Item);
        auto shangxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shangxianwei2Item->setName("$variable$shangxianwei2$");
        shangxianwei2Item->setValueFromDouble(shangxianwei2);
        assembly.addItem(shangxianwei2Item);
        auto xiaxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiaxianwei2Item->setName("$variable$xiaxianwei2$");
        xiaxianwei2Item->setValueFromDouble(xiaxianwei2);
        assembly.addItem(xiaxianwei2Item);
        auto zuoxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zuoxianwei2Item->setName("$variable$zuoxianwei2$");
        zuoxianwei2Item->setValueFromDouble(zuoxianwei2);
        assembly.addItem(zuoxianwei2Item);
        auto youxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        youxianwei2Item->setName("$variable$youxianwei2$");
        youxianwei2Item->setValueFromDouble(youxianwei2);
        assembly.addItem(youxianwei2Item);
        auto baoguang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        baoguang2Item->setName("$variable$baoguang2$");
        baoguang2Item->setValueFromDouble(baoguang2);
        assembly.addItem(baoguang2Item);
        auto zengyi2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zengyi2Item->setName("$variable$zengyi2$");
        zengyi2Item->setValueFromDouble(zengyi2);
        assembly.addItem(zengyi2Item);
        auto xiangsudangliang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangsudangliang2Item->setName("$variable$xiangsudangliang2$");
        xiangsudangliang2Item->setValueFromDouble(xiangsudangliang2);
        assembly.addItem(xiangsudangliang2Item);
        auto changeLanguageIndexItem = std::make_shared<rw::oso::ObjectStoreItem>();
        changeLanguageIndexItem->setName("$variable$changeLanguageIndex$");
        changeLanguageIndexItem->setValueFromInt(changeLanguageIndex);
        assembly.addItem(changeLanguageIndexItem);
        return assembly;
    }

    inline bool SetConfig::operator==(const SetConfig& obj) const
    {
        return xiangjiguangdianpingbishijian == obj.xiangjiguangdianpingbishijian && shuchuxinhaoyanshi == obj.shuchuxinhaoyanshi && shuchuxinhaochixushijian == obj.shuchuxinhaochixushijian && shangxianwei1 == obj.shangxianwei1 && xiaxianwei1 == obj.xiaxianwei1 && zuoxianwei1 == obj.zuoxianwei1 && youxianwei1 == obj.youxianwei1 && baoguang1 == obj.baoguang1 && zengyi1 == obj.zengyi1 && xiangsudangliang1 == obj.xiangsudangliang1 && shangxianwei2 == obj.shangxianwei2 && xiaxianwei2 == obj.xiaxianwei2 && zuoxianwei2 == obj.zuoxianwei2 && youxianwei2 == obj.youxianwei2 && baoguang2 == obj.baoguang2 && zengyi2 == obj.zengyi2 && xiangsudangliang2 == obj.xiangsudangliang2 && changeLanguageIndex == obj.changeLanguageIndex;
    }

    inline bool SetConfig::operator!=(const SetConfig& obj) const
    {
        return !(*this == obj);
    }

}

