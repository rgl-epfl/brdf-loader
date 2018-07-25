#define POWITACQ_IMPLEMENTATION
#include "powitacq.h"


int main(int argc, char **argv) {
    using namespace powitacq;

    // load a BRDF
    BRDF brdf("acrylic_felt_orange_spec.bsdf");

    // evaluate the BRDF
    {
        Vector3f wi = normalize(Vector3f(0, 0, 1));
        Vector3f wo = normalize(Vector3f(1, 1, 1));
        Spectrum lambda = brdf.wavelengths();
        Spectrum fr = brdf.eval(wi, wo);
        /* print values to console */
        for (size_t i = 0; i < fr.size(); ++i)
            printf("%f: %f\n", lambda[i], fr[i]);
    }

    // sample the BRDF
    {
        Vector2f u = Vector2f(0.99334f, 0.231f);
        Vector3f wi = normalize(Vector3f(1, 1, 1));
        Vector3f wo;
        float pdf;

        Spectrum lambda = brdf.wavelengths();
        Spectrum weight = brdf.sample(u, wi, &wo, &pdf);

        /* print values to console */
        for (size_t i = 0; i < weight.size(); ++i)
            printf("%f: %f\n", lambda[i], weight[i]);
    }
}
