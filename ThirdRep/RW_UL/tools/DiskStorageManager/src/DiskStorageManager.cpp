#include "DiskStorageManager.hpp"

#include <iostream>
#include <system_error>

namespace dsm
{
	static constexpr std::uint64_t kGB = 1024ULL * 1024ULL * 1024ULL;

	DiskStorageManager::DiskStorageManager(Config config)
		: m_config(std::move(config))
	{
	}

	std::uint64_t DiskStorageManager::GetDirectorySize(const std::filesystem::path& dir) const
	{
		std::error_code ec;
		if (!std::filesystem::exists(dir, ec) || !std::filesystem::is_directory(dir, ec))
		{
			throw std::runtime_error("Directory does not exist or is not a directory: " + PathForLog(dir));
		}

		std::uint64_t total = 0;
		for (std::filesystem::recursive_directory_iterator it(dir, ec), end; it != end; ++it)
		{
			if (ec)
			{
				throw std::runtime_error("Failed to iterate directory: " + PathForLog(dir) + " - " + ec.message());
			}

			const auto& entry = *it;

			std::error_code sec;
			if (entry.is_symlink(sec))
			{
				// Avoid following symlinks to prevent cycles
				it.disable_recursion_pending();
				continue;
			}
			if (entry.is_regular_file(sec))
			{
				std::error_code fec;
				auto sz = std::filesystem::file_size(entry.path(), fec);
				if (!fec)
				{
					total += static_cast<std::uint64_t>(sz);
				}
			}
		}
		return total;
	}

	bool DiskStorageManager::IsSamePartition(const std::filesystem::path& a, const std::filesystem::path& b) const
	{
#if defined(_WIN32)
		// Windows: judge by root_name (e.g., "C:")
		auto ca = std::filesystem::weakly_canonical(a);
		auto cb = std::filesystem::weakly_canonical(b);
		return ca.root_name() == cb.root_name();
#else
		// *nix: simplified check by root_path; for precise check use statfs (omitted here)
		auto ca = std::filesystem::weakly_canonical(a);
		auto cb = std::filesystem::weakly_canonical(b);
		return ca.root_path() == cb.root_path();
#endif
	}

	std::optional<std::filesystem::directory_entry> DiskStorageManager::FindOldestSubdirectory(const std::filesystem::path& dir) const
	{
		std::error_code ec;
		if (!std::filesystem::exists(dir, ec) || !std::filesystem::is_directory(dir, ec))
		{
			throw std::runtime_error("Directory does not exist or is not a directory: " + PathForLog(dir));
		}

		std::optional<std::filesystem::directory_entry> oldest;
		std::filesystem::file_time_type oldestTime{ std::filesystem::file_time_type::max() };

		for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
		{
			if (ec)
			{
				throw std::runtime_error("Failed to iterate directory: " + PathForLog(dir) + " - " + ec.message());
			}
			if (!entry.is_directory())
			{
				continue;
			}
			std::error_code tec;
			auto ts = std::filesystem::last_write_time(entry, tec);
			if (tec)
			{
				// Skip if last write time is not readable
				continue;
			}
			if (ts < oldestTime)
			{
				oldestTime = ts;
				oldest = entry;
			}
		}
		return oldest;
	}

	void DiskStorageManager::MoveDirectoryAcrossPartitions(const std::filesystem::path& from, const std::filesystem::path& to) const
	{
		std::error_code ec;

		// Avoid overwrite if target path exists; use the same folder name under target dir.
		auto finalTarget = to / from.filename();

		if (std::filesystem::exists(finalTarget, ec))
		{
			throw std::runtime_error("Target already exists, will not overwrite: " + PathForLog(finalTarget));
		}

		// Recursively copy
		std::filesystem::create_directories(to, ec);
		if (ec)
		{
			throw std::runtime_error("Failed to create target directory: " + PathForLog(to) + " - " + ec.message());
		}

		ec.clear();
		std::filesystem::copy(from, finalTarget,
			std::filesystem::copy_options::recursive | std::filesystem::copy_options::copy_symlinks, ec);
		if (ec)
		{
			// Clean up partially copied target
			std::error_code rec;
			std::filesystem::remove_all(finalTarget, rec);
			throw std::runtime_error("Copy failed: " + PathForLog(from) + " -> " + PathForLog(finalTarget) + " - " + ec.message());
		}

		// Remove source
		ec.clear();
		std::filesystem::remove_all(from, ec);
		if (ec)
		{
			throw std::runtime_error("Failed to remove source directory (copy succeeded): " + PathForLog(from) + " - " + ec.message());
		}
	}

	void DiskStorageManager::RemoveDirectoryTree(const std::filesystem::path& dir) const
	{
		std::error_code ec;
		std::filesystem::remove_all(dir, ec);
		if (ec)
		{
			throw std::runtime_error("Failed to remove directory: " + PathForLog(dir) + " - " + ec.message());
		}
	}

	bool DiskStorageManager::RunOnce()
	{
		bool acted = false;

		// If target has a threshold, enforce by deleting its oldest subdir when exceeded
		if (m_config.targetThresholdBytes.has_value())
		{
			const auto targetUsed = GetDirectorySize(m_config.targetDir);
			if (targetUsed >= m_config.targetThresholdBytes.value())
			{
				auto oldestTarget = FindOldestSubdirectory(m_config.targetDir);
				if (oldestTarget.has_value())
				{
					std::cout << "Target exceeds threshold. Deleting oldest folder: " << PathForLog(oldestTarget->path()) << std::endl;
					RemoveDirectoryTree(oldestTarget->path());
					acted = true;
				}
				else
				{
					std::cout << "Target exceeds threshold, but no subdirectory to delete." << std::endl;
				}
			}
		}

		// Partition check before moving from source to target
		if (IsSamePartition(m_config.sourceDir, m_config.targetDir))
		{
			throw std::runtime_error("Source and target directories are on the same partition, but different partitions are required.");
		}

		// If source exceeds threshold, move its oldest subdir to target
		const auto sourceUsed = GetDirectorySize(m_config.sourceDir);
		if (sourceUsed >= m_config.thresholdBytes)
		{
			auto oldestSource = FindOldestSubdirectory(m_config.sourceDir);
			if (!oldestSource.has_value())
			{
				std::cout << "Source exceeds threshold, but no subdirectory available to move." << std::endl;
				return acted;
			}

			std::cout << "Source exceeds threshold. Moving oldest folder: " << PathForLog(oldestSource->path()) << std::endl;
			MoveDirectoryAcrossPartitions(oldestSource->path(), m_config.targetDir);
			std::cout << "Move completed: " << PathForLog(oldestSource->path().filename()) << " -> " << PathForLog(m_config.targetDir) << std::endl;
			acted = true;
		}

		return acted;
	}

	std::string DiskStorageManager::PathForLog(const std::filesystem::path& p)
	{
		return p.u8string();
	}
}