#pragma once

#include"oso_core.h"
#include <string>

namespace cdm {
    class MaiLiDingZiConfig
    {
    public:
        MaiLiDingZiConfig() = default;
        ~MaiLiDingZiConfig() = default;

        MaiLiDingZiConfig(const rw::oso::ObjectStoreAssembly& assembly);
        MaiLiDingZiConfig(const MaiLiDingZiConfig& obj);

        MaiLiDingZiConfig& operator=(const MaiLiDingZiConfig& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const MaiLiDingZiConfig& obj) const;
        bool operator!=(const MaiLiDingZiConfig& obj) const;

    public:
        int totalDefectiveVolume{ 0 };
        bool isDebug{ false };
        bool isDefect{ false };
        bool isSaveImg{ false };
    };

    inline MaiLiDingZiConfig::MaiLiDingZiConfig(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$MaiLiDingZiConfig$")
        {
            throw std::runtime_error("Assembly is not $class$MaiLiDingZiConfig$");
        }
        auto totalDefectiveVolumeItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$totalDefectiveVolume$"));
        if (!totalDefectiveVolumeItem) {
            throw std::runtime_error("$variable$totalDefectiveVolume is not found");
        }
        totalDefectiveVolume = totalDefectiveVolumeItem->getValueAsInt();
        auto isDebugItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$isDebug$"));
        if (!isDebugItem) {
            throw std::runtime_error("$variable$isDebug is not found");
        }
        isDebug = isDebugItem->getValueAsBool();
        auto isDefectItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$isDefect$"));
        if (!isDefectItem) {
            throw std::runtime_error("$variable$isDefect is not found");
        }
        isDefect = isDefectItem->getValueAsBool();
        auto isSaveImgItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$isSaveImg$"));
        if (!isSaveImgItem) {
            throw std::runtime_error("$variable$isSaveImg is not found");
        }
        isSaveImg = isSaveImgItem->getValueAsBool();
    }

    inline MaiLiDingZiConfig::MaiLiDingZiConfig(const MaiLiDingZiConfig& obj)
    {
        totalDefectiveVolume = obj.totalDefectiveVolume;
        isDebug = obj.isDebug;
        isDefect = obj.isDefect;
        isSaveImg = obj.isSaveImg;
    }

    inline MaiLiDingZiConfig& MaiLiDingZiConfig::operator=(const MaiLiDingZiConfig& obj)
    {
        if (this != &obj) {
            totalDefectiveVolume = obj.totalDefectiveVolume;
            isDebug = obj.isDebug;
            isDefect = obj.isDefect;
            isSaveImg = obj.isSaveImg;
        }
        return *this;
    }

    inline MaiLiDingZiConfig::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$MaiLiDingZiConfig$");
        auto totalDefectiveVolumeItem = std::make_shared<rw::oso::ObjectStoreItem>();
        totalDefectiveVolumeItem->setName("$variable$totalDefectiveVolume$");
        totalDefectiveVolumeItem->setValueFromInt(totalDefectiveVolume);
        assembly.addItem(totalDefectiveVolumeItem);
        auto isDebugItem = std::make_shared<rw::oso::ObjectStoreItem>();
        isDebugItem->setName("$variable$isDebug$");
        isDebugItem->setValueFromBool(isDebug);
        assembly.addItem(isDebugItem);
        auto isDefectItem = std::make_shared<rw::oso::ObjectStoreItem>();
        isDefectItem->setName("$variable$isDefect$");
        isDefectItem->setValueFromBool(isDefect);
        assembly.addItem(isDefectItem);
        auto isSaveImgItem = std::make_shared<rw::oso::ObjectStoreItem>();
        isSaveImgItem->setName("$variable$isSaveImg$");
        isSaveImgItem->setValueFromBool(isSaveImg);
        assembly.addItem(isSaveImgItem);
        return assembly;
    }

    inline bool MaiLiDingZiConfig::operator==(const MaiLiDingZiConfig& obj) const
    {
        return totalDefectiveVolume == obj.totalDefectiveVolume && isDebug == obj.isDebug && isDefect == obj.isDefect && isSaveImg == obj.isSaveImg;
    }

    inline bool MaiLiDingZiConfig::operator!=(const MaiLiDingZiConfig& obj) const
    {
        return !(*this == obj);
    }

}

