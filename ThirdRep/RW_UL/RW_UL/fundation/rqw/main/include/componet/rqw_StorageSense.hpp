#pragma once

#include <QString>
#include <QtGlobal>
#include <QStorageInfo>

namespace rw
{
    namespace rqw
    {

        /**
         * @brief Enumeration for storage units.
         * Used to specify the unit for space calculation.
         */
        enum class StorageUnit
        {
            Byte,   ///< Bytes
            KB,     ///< Kilobytes
            MB,     ///< Megabytes
            GB,     ///< Gigabytes
            TB      ///< Terabytes
        };

        /**
         * @brief StorageSense provides disk and folder space information for a given path.
         * It uses Qt's QStorageInfo to query disk space and supports unit conversion.
         */
        class StorageSense
        {
        public:
            /**
             * @brief Constructor.
             * @param path The path to be monitored for storage information.
             */
            explicit StorageSense(const QString& path);

        private:
            QString _path; ///< The monitored path.

        public:
            //For the partition where the folder is located
            /**
             * @brief Get available free space in bytes.
             * @return Available space in bytes.
             */
            qint64 getAvailableSpace() const;

            /**
             * @brief Get total space in bytes.
             * @return Total space in bytes.
             */
            qint64 getTotalSpace() const;

            /**
             * @brief Get used space in bytes.
             * @return Used space in bytes.
             */
            qint64 getUsedSpace() const;

            /**
             * @brief Get available space in specified unit.
             * @param unit The unit to convert to.
             * @return Available space in the specified unit.
             */
            double getAvailableSpace(StorageUnit unit) const;

            /**
             * @brief Get total space in specified unit.
             * @param unit The unit to convert to.
             * @return Total space in the specified unit.
             */
            double getTotalSpace(StorageUnit unit) const;

            /**
             * @brief Get used space in specified unit.
             * @param unit The unit to convert to.
             * @return Used space in the specified unit.
             */
            double getUsedSpace(StorageUnit unit) const;
        public:
			//For the folder at the monitored path
            /**
             * @brief Get the size of the folder at the monitored path in bytes.
             * @return Folder size in bytes.
             */
            qint64 getFolderSize() const;

            /**
             * @brief Get the size of the folder at the monitored path in specified unit.
             * @param unit The unit to convert to.
             * @return Folder size in the specified unit.
             */
            double getFolderSize(StorageUnit unit) const;

        private:
            QStorageInfo m_storage; ///< Qt storage info object.

            /**
             * @brief Convert bytes to the specified unit.
             * @param bytes The value in bytes.
             * @param unit The unit to convert to.
             * @return Converted value as double.
             */
            static double convertUnit(qint64 bytes, StorageUnit unit);
        };
    }
}