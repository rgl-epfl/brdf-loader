#define POWITACQ_IMPLEMENTATION
#include "powitacq_rgb.h"


int main(int argc, char **argv) {
    using namespace powitacq;

    // load a BRDF
    BRDF brdf("cc_ibiza_sunset_rgb.bsdf");

    // evaluate the BRDF
    {
        Vector3f wi = normalize(Vector3f(0, 0, 1));
        Vector3f wo = normalize(Vector3f(1, 1, 1));
        Vector3f fr = brdf.eval(wi, wo);
        /* print values to console */
        printf("%f %f %f\n", fr[0], fr[1], fr[2]);
    }

    // sample the BRDF
    {
        Vector2f u = Vector2f(0.99334f, 0.231f);
        Vector3f wi = normalize(Vector3f(1, 1, 1));
        Vector3f wo;
        float pdf;

        Vector3f fr = brdf.sample(u, wi, &wo, &pdf);

        /* print values to console */
        printf("%f %f %f\n", fr[0], fr[1], fr[2]);
    }
}
