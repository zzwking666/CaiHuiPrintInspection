#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include "dsl_CacheFactory.hpp"

namespace py = pybind11;
using namespace rw::dsl;

PYBIND11_MODULE(dslPy, m) {
    py::enum_<CachePolicy>(m, "CachePolicy")
        .value("LRU", CachePolicy::LRU)
        .value("LFU", CachePolicy::LFU)
        .value("MRU", CachePolicy::MRU)
        .value("MFU", CachePolicy::MFU)
        .value("CLOCK", CachePolicy::Clock)
        .export_values();

    py::class_<ICache<int, int>, std::shared_ptr<ICache<int, int>>>(m, "ICache")
        .def("get", &ICache<int, int>::get)
        .def("set", &ICache<int, int>::set);

    py::class_<CacheLMRU<int, int>, ICache<int, int>, std::shared_ptr<CacheLMRU<int, int>>>(m, "CacheLMRU")
        .def(py::init<size_t>());

    py::class_<CacheLMFU<int, int>, ICache<int, int>, std::shared_ptr<CacheLMFU<int, int>>>(m, "CacheLMFU")
        .def(py::init<size_t>());

    py::class_<CacheClock<int, int>, ICache<int, int>, std::shared_ptr<CacheClock<int, int>>>(m, "CacheClock")
        .def(py::init<size_t>());

    py::class_<CacheFactory>(m, "CacheFactory")
        .def_static("createCache", [](CachePolicy policy, size_t capacity) {
        return CacheFactory::createCache<int, int>(policy, capacity);
            }, py::arg("policy"), py::arg("capacity"));
}