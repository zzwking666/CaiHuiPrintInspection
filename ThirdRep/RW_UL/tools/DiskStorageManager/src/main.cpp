#include "DiskStorageManager.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cctype>
#include <optional>
#include <atomic>
#include <csignal>
#include <thread>
#include <chrono>

using namespace dsm;

static constexpr std::uint64_t kGB = 1024ULL * 1024ULL * 1024ULL;

static std::atomic<bool> g_stop{ false };

static void SignalHandler(int)
{
	g_stop.store(true);
}

// Simple argument parsing: --source <dir> --target <dir> [--threshold <number>[g|m]] [--target-threshold <number>[g|m]] [--interval <seconds>]
struct Args
{
	std::optional<std::filesystem::path> source;
	std::optional<std::filesystem::path> target;
	std::optional<std::uint64_t> thresholdBytes;
	std::optional<std::uint64_t> targetThresholdBytes;
	std::optional<unsigned int> intervalSeconds;
};

static void PrintUsage()
{
	std::cout << "Usage:\n"
		<< "  DiskStorageManager --source <source_dir> --target <target_dir> [--threshold <value>[g|m]] [--target-threshold <value>[g|m]] [--interval <seconds>]\n\n"
		<< "Description:\n"
		<< "  --source            Required. Directory to monitor (source).\n"
		<< "  --target            Required. Destination directory (target). Must be on a different partition than source.\n"
		<< "  --threshold         Optional. Threshold on source directory size. Unit: g(GB) or m(MB). Default: 50g.\n"
		<< "  --target-threshold  Optional. Threshold on target directory size. Unit: g(GB) or m(MB). If set and exceeded, deletes oldest subdirectory in target.\n"
		<< "  --interval          Optional. Check interval in seconds. Default is 60.\n"
		<< std::endl;
}

static std::optional<std::uint64_t> ParseSizeToBytes(const std::string& s)
{
	if (s.empty())
	{
		return std::nullopt;
	}
	std::size_t i = 0;
	while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
	{
		++i;
	}
	if (i == 0)
	{
		return std::nullopt;
	}
	std::uint64_t value = 0;
	try
	{
		value = std::stoull(s.substr(0, i));
	}
	catch (...)
	{
		return std::nullopt;
	}
	std::string unit = s.substr(i);
	for (auto& c : unit)
	{
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	if (unit.empty() || unit == "b")
	{
		return value;
	}
	if (unit == "m")
	{
		return value * 1024ULL * 1024ULL;
	}
	if (unit == "g")
	{
		return value * kGB;
	}
	return std::nullopt;
}

static Args ParseArgs(int argc, char** argv)
{
	Args args;
	std::vector<std::string> tokens;
	tokens.reserve(static_cast<std::size_t>(argc));
	for (int i = 0; i < argc; ++i)
	{
		tokens.emplace_back(argv[i]);
	}

	for (std::size_t i = 1; i < tokens.size(); ++i)
	{
		const auto& t = tokens[i];
		if (t == "--source" && (i + 1) < tokens.size())
		{
			args.source = std::filesystem::path(tokens[++i]);
		}
		else if (t == "--target" && (i + 1) < tokens.size())
		{
			args.target = std::filesystem::path(tokens[++i]);
		}
		else if (t == "--threshold" && (i + 1) < tokens.size())
		{
			auto val = ParseSizeToBytes(tokens[++i]);
			if (!val.has_value())
			{
				throw std::runtime_error("Invalid threshold format. Example: 50g or 1024m");
			}
			args.thresholdBytes = val.value();
		}
		else if (t == "--target-threshold" && (i + 1) < tokens.size())
		{
			auto val = ParseSizeToBytes(tokens[++i]);
			if (!val.has_value())
			{
				throw std::runtime_error("Invalid target threshold format. Example: 200g");
			}
			args.targetThresholdBytes = val.value();
		}
		else if (t == "--interval" && (i + 1) < tokens.size())
		{
			const std::string v = tokens[++i];
			try
			{
				unsigned long long secs = std::stoull(v);
				if (secs == 0ULL || secs > static_cast<unsigned long long>(std::numeric_limits<unsigned int>::max()))
				{
					throw std::runtime_error("Interval must be between 1 and UINT_MAX seconds.");
				}
				args.intervalSeconds = static_cast<unsigned int>(secs);
			}
			catch (...)
			{
				throw std::runtime_error("Invalid interval format. Example: 60");
			}
		}
		else if (t == "--help" || t == "-h")
		{
			PrintUsage();
			std::exit(0);
		}
		else
		{
			throw std::runtime_error(std::string("Unknown argument: ") + t);
		}
	}

	return args;
}

int main(int argc, char** argv)
{
	try
	{
		// Register signal handlers for graceful shutdown (Ctrl+C)
		std::signal(SIGINT, SignalHandler);
#if defined(_WIN32)
		std::signal(SIGBREAK, SignalHandler);
#endif
		std::signal(SIGTERM, SignalHandler);

		auto args = ParseArgs(argc, argv);

		if (!args.source.has_value() || !args.target.has_value())
		{
			PrintUsage();
			std::cerr << "Error: --source and --target are required.\n";
			return 2;
		}

		// Default threshold: 50GB (source)
		std::uint64_t threshold = args.thresholdBytes.has_value() ? args.thresholdBytes.value() : (50ULL * kGB);
		// Default interval: 60s
		unsigned int intervalSeconds = args.intervalSeconds.has_value() ? args.intervalSeconds.value() : 60U;

		dsm::Config cfg{};
		cfg.sourceDir = std::filesystem::weakly_canonical(args.source.value());
		cfg.targetDir = std::filesystem::weakly_canonical(args.target.value());
		cfg.thresholdBytes = threshold;
		cfg.targetThresholdBytes = args.targetThresholdBytes;

		DiskStorageManager manager(cfg);

		std::cout << "DiskStorageManager started. Checking every " << intervalSeconds << " seconds.\n";
		std::cout << "Source: " << cfg.sourceDir.u8string() << "\n";
		std::cout << "Target: " << cfg.targetDir.u8string() << "\n";
		std::cout << "Source threshold: " << threshold << " bytes\n";
		if (cfg.targetThresholdBytes.has_value())
		{
			std::cout << "Target threshold: " << cfg.targetThresholdBytes.value() << " bytes\n";
		}
		else
		{
			std::cout << "Target threshold: not set\n";
		}

		while (!g_stop.load())
		{
			try
			{
				(void)manager.RunOnce();
			}
			catch (const std::exception& ex)
			{
				std::cerr << "RunOnce error: " << ex.what() << std::endl;
			}

			const unsigned int stepMs = 200;
			unsigned int waitedMs = 0;
			while (!g_stop.load() && waitedMs < (intervalSeconds * 1000U))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(stepMs));
				waitedMs += stepMs;
			}
		}

		std::cout << "DiskStorageManager stopping..." << std::endl;
		return 0;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}
}