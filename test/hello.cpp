
#define POWITACQ_BRDF_IMPLEMENTATION
#include "brdf.h"

int main(int argc, char **argv)
{
    // load a BRDF
    powitacq::brdf brdf("/home/jdups/Dropbox/powitacq_datasets/materials/acrylic_felt_orange_spec.bsdf");

    // evaluate the BRDF
    {
        powitacq::vec3 wi = powitacq::normalize(powitacq::vec3(0, 0, 1));
        powitacq::vec3 wo = powitacq::normalize(powitacq::vec3(1, 1, 1));
        powitacq::brdf::value_type fr = brdf.eval(wi, wo);
        /* print values to console */
        for (int i = 0; i < fr.size(); ++i) {
            printf("%f\n", fr[i]);
        }
    }

    // sample the BRDF
    {
        powitacq::vec2 u = powitacq::vec2(0.99334f, 0.231f);
        powitacq::vec3 wi = powitacq::normalize(powitacq::vec3(1, 1, 1));
        powitacq::brdf::value_type fr = brdf.sample(u, wi);
        /* print values to console */
        for (int i = 0; i < fr.size(); ++i) {
            printf("%f\n", fr[i]);
        }
    }
}