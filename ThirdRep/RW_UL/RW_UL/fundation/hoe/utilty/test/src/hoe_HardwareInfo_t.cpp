#include<gtest/gtest.h>

namespace hoe_hardwareInfo
{
	//TEST(HwlocTest, GetTopology) {
	//	hwloc_topology_t topology;
	//	hwloc_topology_init(&topology);
	//	hwloc_topology_load(topology);

	//	// 获取 CPU 型号信息
	//	hwloc_obj_t cpu_obj = hwloc_get_root_obj(topology);
	//	if (cpu_obj) {
	//		const char* model_name = hwloc_obj_get_info_by_name(cpu_obj, "ProcessorName");
	//		if (model_name) {
	//			std::cout << "CPU Model: " << model_name << std::endl;
	//		}
	//		else {
	//			model_name = hwloc_obj_get_info_by_name(cpu_obj, "ModelName");
	//			if (model_name) {
	//				std::cout << "CPU Model: " << model_name << std::endl;
	//			}
	//			else {
	//				// 尝试从第一个 PU 对象获取信息
	//				hwloc_obj_t pu_obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, 0);
	//				if (pu_obj) {
	//					model_name = hwloc_obj_get_info_by_name(pu_obj, "ModelName");
	//					if (model_name) {
	//						std::cout << "CPU Model (from PU): " << model_name << std::endl;
	//					}
	//					else {
	//						std::cout << "CPU Model information not found." << std::endl;
	//					}
	//				}
	//				else {
	//					std::cout << "CPU Model information not found." << std::endl;
	//				}
	//			}
	//		}
	//	}
	//	else {
	//		std::cout << "Could not retrieve CPU object." << std::endl;
	//	}

	//	hwloc_topology_destroy(topology);
	//}
}