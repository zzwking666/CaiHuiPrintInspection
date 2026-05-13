#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include <chrono>

namespace dsm
{
	struct Config
	{
		std::filesystem::path sourceDir;
		std::filesystem::path targetDir;
		std::uint64_t thresholdBytes; // threshold for source directory size
		std::optional<std::uint64_t> targetThresholdBytes; // optional threshold for target directory size
	};

	class DiskStorageManager
	{
	public:
		explicit DiskStorageManager(Config config);

		// Perform a single check:
		// - If source size >= thresholdBytes: move oldest subdirectory from source to target.
		// - If targetThresholdBytes is set and target size >= targetThresholdBytes: delete oldest subdirectory in target.
		// Returns true if any action was performed.
		bool RunOnce();

	private:
		Config m_config;

		// Compute total size (bytes) of a directory (recursive).
		std::uint64_t GetDirectorySize(const std::filesystem::path& dir) const;

		// Check if two paths are on the same partition.
		bool IsSamePartition(const std::filesystem::path& a, const std::filesystem::path& b) const;

		// Find the oldest (by last write time) immediate subdirectory under 'dir'.
		std::optional<std::filesystem::directory_entry> FindOldestSubdirectory(const std::filesystem::path& dir) const;

		// Cross-partition move: recursively copy then remove source.
		void MoveDirectoryAcrossPartitions(const std::filesystem::path& from, const std::filesystem::path& to) const;

		// Remove a directory tree.
		void RemoveDirectoryTree(const std::filesystem::path& dir) const;

		// Path to string for logging.
		static std::string PathForLog(const std::filesystem::path& p);
	};
}