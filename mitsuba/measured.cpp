/*
    This file provides a Mitsuba implementation of the BRDF model. It
    is mostly just a thin wrapper around "powitacq_rgb.h".
*/

#include <mitsuba/core/fresolver.h>
#include <mitsuba/render/bsdf.h>
#include <mitsuba/hw/basicshader.h>
#include "microfacet.h"
#include "ior.h"

#define POWITACQ_IMPLEMENTATION
#include "powitacq_rgb.h"

MTS_NAMESPACE_BEGIN

class Measured : public BSDF {
public:
    Measured(const Properties &props):
        BSDF(props) {
        fs::path filename = Thread::getThread()->getFileResolver()->resolve(
            props.getString("filename"));

        m_brdf = new powitacq::BRDF(filename.string().c_str());
    }

    Measured(Stream *stream, InstanceManager *manager)
     : BSDF(stream, manager) {
        configure();
    }

    ~Measured() {
        delete m_brdf;
    }

    void serialize(Stream *stream, InstanceManager *manager) const {
        BSDF::serialize(stream, manager);
    }

    void configure() {
        m_components.push_back(EDiffuseReflection | EFrontSide | 0);
        BSDF::configure();
    }

    Spectrum eval(const BSDFSamplingRecord &bRec, EMeasure measure) const {
        powitacq::Vector3f wi = powitacq::Vector3f(bRec.wi.x, bRec.wi.y, bRec.wi.z);
        powitacq::Vector3f wo = powitacq::Vector3f(bRec.wo.x, bRec.wo.y, bRec.wo.z);
        powitacq::Vector3f fr = m_brdf->eval(wi, wo);

        return Color3(fr[0], fr[1], fr[2]);
    }

    Float pdf(const BSDFSamplingRecord &bRec, EMeasure measure) const {
        powitacq::Vector3f wi = powitacq::Vector3f(bRec.wi.x, bRec.wi.y, bRec.wi.z);
        powitacq::Vector3f wo = powitacq::Vector3f(bRec.wo.x, bRec.wo.y, bRec.wo.z);

        return m_brdf->pdf(wi, wo);
    }

    Spectrum sample(BSDFSamplingRecord &bRec, Float &pdf, const Point2 &sample) const {
        powitacq::Vector3f wi = powitacq::Vector3f(bRec.wi.x, bRec.wi.y, bRec.wi.z);
        powitacq::Vector2f u = powitacq::Vector2f(sample.x, sample.y);
        powitacq::Vector3f wo;
        powitacq::Vector3f fr = m_brdf->sample(u, wi, &wo, &pdf);

        // set out dir
        bRec.wo = Vector3f(wo[0], wo[1], wo[2]);
        bRec.eta = 1.0f;
        bRec.sampledComponent = 0;
        bRec.sampledType = EDiffuseReflection;

        return Color3(fr[0], fr[1], fr[2]);
    }

    Spectrum sample(BSDFSamplingRecord &bRec, const Point2 &sample) const {
        Float pdf_ = 0.f;
        return this->sample(bRec, pdf_, sample);
    }

    std::string toString() const {
        return "Measured[];"
    }

    Shader *createShader(Renderer *renderer) const;

    MTS_DECLARE_CLASS()
private:
    powitacq::BRDF *m_brdf;
};

// ================ Hardware shader implementation ================
class MeasuredShader : public Shader {
public:
    MeasuredShader(Renderer *renderer)
        : Shader(renderer, EBSDFShader) {
    }

    bool isComplete() const { return true; }
    void cleanup(Renderer *renderer) {}
    void putDependencies(std::vector<Shader *> &deps) { }

    void generateCode(std::ostringstream &oss,
            const std::string &evalName,
            const std::vector<std::string> &depNames) const {
        oss << "vec3 " << evalName << "(vec2 uv, vec3 wi, vec3 wo) {" << endl
            << "    if (cosTheta(wi) < 0.0 || cosTheta(wo) < 0.0)" << endl
            << "        return vec3(0.0);" << endl
            << "    return vec3(inv_pi * cosTheta(wo));" << endl
            << "}" << endl
            << endl
            << "vec3 " << evalName << "_diffuse(vec2 uv, vec3 wi, vec3 wo) {" << endl
            << "    return " << evalName << "(uv, wi, wo);" << endl
            << "}" << endl;
    }

    MTS_DECLARE_CLASS()
private:
    ref<Shader> m_reflectanceShader;
};

Shader *Measured::createShader(Renderer *renderer) const {
    return new MeasuredShader(renderer);
}

MTS_IMPLEMENT_CLASS(MeasuredShader, false, Shader)
MTS_IMPLEMENT_CLASS_S(Measured, false, BSDF)
MTS_EXPORT_PLUGIN(Measured, "Measured BRDF");
MTS_NAMESPACE_END
