#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/string.h>

#define POWITACQ_IMPLEMENTATION 1
#include "powitacq_rgb.h"

namespace nb = nanobind;
namespace powi = powitacq_rgb;

using namespace nb::literals;


template <typename Type, size_t Dim, typename Cls>
void bind_vector(Cls& cls){
    using Vec = powi::Vector<Type, Dim>;
    cls.def(nb::init<>())
        .def(nb::init<Type>())
        .def(nb::init<std::array<Type, Dim>>())
        .def("__repr__", [](Vec& self){
            std::ostringstream oss;
            oss << "[";
            for(size_t i = 0; i < Dim; ++i){
                oss << (self[i]) << (i == (Dim-1) ? "" : ", ");
            }
            oss << "]";
            return oss.str();
        });

    if constexpr (Dim == 2){
        cls.def(nb::init<Type, Type>());
    } else if constexpr (Dim == 3){
        cls.def(nb::init<Type, Type, Type>());
    }
}

NB_MODULE(brdf_loader_ext, m) {
    auto vec2 = nb::class_<powi::Vector2f>(m, "Vector2f");
    bind_vector<float, 2>(vec2);

    auto vec3 = nb::class_<powi::Vector3f>(m, "Vector3f");
    bind_vector<float, 3>(vec3);

    nb::class_<powi::BRDF>(m,"BRDF_rgb")
        .def(nb::init<const std::string&>())
        .def("eval", &powi::BRDF::eval, "wi"_a, "wo"_a)
        .def("sample", [](powi::BRDF* self, const powi::Vector2f &u, const powi::Vector3f &wi) -> nb::tuple {
            powi::Vector3f wo;
            float pdf;
            auto color = self->sample(u, wi, &wo, &pdf);
            return nb::make_tuple(color, wo, pdf);
        })
        .def("pdf", &powi::BRDF::pdf, "wi"_a, "wo"_a);
}
