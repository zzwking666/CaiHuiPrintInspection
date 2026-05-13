#include"rqw_StorageSense.hpp"


namespace rw
{
	namespace rqw
	{
        StorageSense::StorageSense(const QString& path)
            : m_storage(path),_path(path)
        {
            m_storage.refresh();
        }

        qint64 StorageSense::getAvailableSpace() const
        {
            return m_storage.bytesAvailable();
        }

        qint64 StorageSense::getTotalSpace() const
        {
            return m_storage.bytesTotal();
        }

        qint64 StorageSense::getUsedSpace() const
        {
            return m_storage.bytesTotal() - m_storage.bytesAvailable();
        }

        double StorageSense::convertUnit(qint64 bytes, StorageUnit unit)
        {
            switch (unit)
            {
            case StorageUnit::Byte:
                return static_cast<double>(bytes);
            case StorageUnit::KB:
                return static_cast<double>(bytes) / 1024.0;
            case StorageUnit::MB:
                return static_cast<double>(bytes) / (1024.0 * 1024.0);
            case StorageUnit::GB:
                return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
            case StorageUnit::TB:
                return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0 * 1024.0);
            default:
                return static_cast<double>(bytes);
            }
        }

        double StorageSense::getAvailableSpace(StorageUnit unit) const
        {
            return convertUnit(getAvailableSpace(), unit);
        }

        double StorageSense::getTotalSpace(StorageUnit unit) const
        {
            return convertUnit(getTotalSpace(), unit);
        }

        double StorageSense::getUsedSpace(StorageUnit unit) const
        {
            return convertUnit(getUsedSpace(), unit);
        }

        static qint64 calculateFolderSize(const QString& path)
        {
            qint64 totalSize = 0;
            QDir dir(path);

            if (!dir.exists())
                return 0;

            QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
            for (const QFileInfo& fileInfo : files)
            {
                totalSize += fileInfo.size();
            }

            QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QFileInfo& dirInfo : dirs)
            {
                totalSize += calculateFolderSize(dirInfo.absoluteFilePath());
            }

            return totalSize;
        }

        qint64 StorageSense::getFolderSize() const
        {
            return calculateFolderSize(_path);
        }

        double StorageSense::getFolderSize(StorageUnit unit) const
        {
            return convertUnit(getFolderSize(), unit);
        }
	}
}
