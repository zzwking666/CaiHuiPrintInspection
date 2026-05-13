#include"hoei_MotherBoardInfo.hpp"

#include <sstream>
#include <stdexcept>

namespace rw
{
	namespace hoei
	{
		std::string MotherBoardInfoFactory::GetMotherboardUniqueID()
		{
			//#if defined(_WIN32)
						// Windows-specific implementation using WMI
			try {
				// Execute the WMIC command to retrieve the UUID
				char buffer[128];
				FILE* pipe = _popen("wmic csproduct get UUID", "r");
				if (!pipe) {
					throw std::runtime_error("Failed to execute WMIC command.");
				}

				std::string result;
				while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
					result += buffer;
				}
				_pclose(pipe);

				// Parse the result to extract the UUID
				std::istringstream stream(result);
				std::string line;
				std::getline(stream, line); // Skip the header
				std::getline(stream, line); // Get the UUID

				// Trim whitespace and special characters
				line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
				return line.empty() ? "Unknown UUID" : line;
			}
			catch (const std::exception& e) {
				throw std::runtime_error("Failed to retrieve motherboard UUID: " + std::string(e.what()));
			}
			//#elif defined(__linux__)
			//			// Linux-specific implementation using system commands
			//			try {
			//				// Placeholder for Linux implementation
			//				return "Linux_Motherboard_ID";
			//			}
			//			catch (const std::exception& e) {
			//				throw std::runtime_error("Failed to retrieve motherboard UUID: " + std::string(e.what()));
			//			}
			//#else
			//			throw std::runtime_error("Unsupported platform for retrieving motherboard UUID.");
			//#endif
		}

		MotherBoardInfo::MotherBoardInfo(const MotherBoardInfo& other)
			: UUID(other.UUID) {
		}

		MotherBoardInfo::MotherBoardInfo(MotherBoardInfo&& other) noexcept
			: UUID(std::move(other.UUID)) {
		}

		MotherBoardInfo& MotherBoardInfo::operator=(const MotherBoardInfo& other)
		{
			if (this != &other) {
				UUID = other.UUID;
			}
			return *this;
		}

		MotherBoardInfo& MotherBoardInfo::operator=(MotherBoardInfo&& other) noexcept
		{
			if (this != &other) {
				UUID = std::move(other.UUID);
			}
			return *this;
		}

		void MotherBoardInfo::getCurrentContextInfo()
		{
			auto info = MotherBoardInfoFactory::createMotherBoardInfo();
			*this = info;
		}

		MotherBoardInfo MotherBoardInfoFactory::createMotherBoardInfo()
		{
			MotherBoardInfo info;
			info.UUID = MotherBoardInfoFactory::GetMotherboardUniqueID();
			return info;
		}
	}
}