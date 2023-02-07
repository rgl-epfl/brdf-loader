#include <nanobind/nanobind.h>

#define POWITACQ_IMPLEMENTATION 1
#include "powitacq_rgb.h"

namespace nb = nanobind;

using namespace nb::literals;

NB_MODULE(brdf_loader_ext, m) {
    m.def("add", [](int a, int b) { return a + b; }, "a"_a, "b"_a);
}
