#pragma once

#include"oso_core.h"
#include <string>

namespace cdm {
    class CaiHuiPrintInspectionConfig
    {
    public:
        CaiHuiPrintInspectionConfig() = default;
        ~CaiHuiPrintInspectionConfig() = default;

        CaiHuiPrintInspectionConfig(const rw::oso::ObjectStoreAssembly& assembly);
        CaiHuiPrintInspectionConfig(const CaiHuiPrintInspectionConfig& obj);

        CaiHuiPrintInspectionConfig& operator=(const CaiHuiPrintInspectionConfig& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const CaiHuiPrintInspectionConfig& obj) const;
        bool operator!=(const CaiHuiPrintInspectionConfig& obj) const;

    public:
        int totalDefectiveVolume{ 0 };
        bool isDebug{ false };
        bool isDefect{ false };
        bool isSaveImg{ false };
    };

    inline CaiHuiPrintInspectionConfig::CaiHuiPrintInspectionConfig(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$CaiHuiPrintInspectionConfig$")
        {
            throw std::runtime_error("Assembly is not $class$CaiHuiPrintInspectionConfig$");
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

    inline CaiHuiPrintInspectionConfig::CaiHuiPrintInspectionConfig(const CaiHuiPrintInspectionConfig& obj)
    {
        totalDefectiveVolume = obj.totalDefectiveVolume;
        isDebug = obj.isDebug;
        isDefect = obj.isDefect;
        isSaveImg = obj.isSaveImg;
    }

    inline CaiHuiPrintInspectionConfig& CaiHuiPrintInspectionConfig::operator=(const CaiHuiPrintInspectionConfig& obj)
    {
        if (this != &obj) {
            totalDefectiveVolume = obj.totalDefectiveVolume;
            isDebug = obj.isDebug;
            isDefect = obj.isDefect;
            isSaveImg = obj.isSaveImg;
        }
        return *this;
    }

    inline CaiHuiPrintInspectionConfig::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$CaiHuiPrintInspectionConfig$");
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

    inline bool CaiHuiPrintInspectionConfig::operator==(const CaiHuiPrintInspectionConfig& obj) const
    {
        return totalDefectiveVolume == obj.totalDefectiveVolume && isDebug == obj.isDebug && isDefect == obj.isDefect && isSaveImg == obj.isSaveImg;
    }

    inline bool CaiHuiPrintInspectionConfig::operator!=(const CaiHuiPrintInspectionConfig& obj) const
    {
        return !(*this == obj);
    }

}

