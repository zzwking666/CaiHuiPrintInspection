#pragma once

#include "imgPro_DefectResultInfoFunc.hpp"
#include "imgPro_EliminationInfoFunc.hpp"
#include "SetConfig.hpp"

namespace cdm
{
	struct ScoreConfigConvert
	{
		static rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap toClassIdWithEliConfigMap(double areaFactor, double scoreFactor);
		static rw::imgPro::DefectResultInfoFunc::ClassIdWithConfigMap toClassIdWithDefConfigMap();
	};
}
