#include "Utilty.hpp"

QString Utility::cameraIp1 = "1";
QString Utility::cameraIp2 = "2";

std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> ClassId::classIdNameMap = {
	{ClassId::dingZi,"钉子"}
};

std::vector<rw::imgPro::ClassId> ClassId::classids = {
	ClassId::dingZi
};