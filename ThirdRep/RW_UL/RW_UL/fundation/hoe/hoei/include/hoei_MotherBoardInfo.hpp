#pragma once

#include <string>
#include <vector>

namespace rw {
	namespace hoei {
		struct MotherBoardInfo
		{
		public:
			MotherBoardInfo() = default;

			MotherBoardInfo(const MotherBoardInfo& other);

			MotherBoardInfo(MotherBoardInfo&& other) noexcept;

			MotherBoardInfo& operator=(const MotherBoardInfo& other);

			MotherBoardInfo& operator=(MotherBoardInfo&& other) noexcept;
		public:
			void getCurrentContextInfo();
		public:
			std::string UUID{};
		};

		class MotherBoardInfoFactory
		{
		public:
			static MotherBoardInfo createMotherBoardInfo();
		public:
			static std::string GetMotherboardUniqueID();
		};
	}
}