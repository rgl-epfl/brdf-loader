/*
    This file is part of Mitsuba, a physically based rendering system.

    Copyright (c) 2007-2014 by Wenzel Jakob and others.

    Mitsuba is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Mitsuba is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <mitsuba/core/fresolver.h>
#include <mitsuba/render/bsdf.h>
#include <mitsuba/hw/basicshader.h>
#include "microfacet.h"
#include "ior.h"

#define POWITACQ_IMPLEMENTATION
#include "powitacq_rgb.h"

MTS_NAMESPACE_BEGIN

class Powitacq : public BSDF {
public:
    Powitacq(const Properties &props):
        BSDF(props) {
        fs::path filename =
                Thread::getThread()->getFileResolver()->resolve(props.getString("filename"));

        // load file
        m_brdf = new powitacq::BRDF(filename.string().c_str());
    }

    Powitacq(Stream *stream, InstanceManager *manager)
     : BSDF(stream, manager) {
        configure();
    }

    ~Powitacq()
    {
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
        std::ostringstream oss;
        oss << "Powitacq[" << endl
            << "]";
        return oss.str();
    }

    Shader *createShader(Renderer *renderer) const;

    MTS_DECLARE_CLASS()
private:
    powitacq::BRDF *m_brdf;
};

// ================ Hardware shader implementation ================
class PowitacqShader : public Shader {
public:
    PowitacqShader(Renderer *renderer)
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

Shader *Powitacq::createShader(Renderer *renderer) const {
    return new PowitacqShader(renderer);
}

MTS_IMPLEMENT_CLASS(PowitacqShader, false, Shader)
MTS_IMPLEMENT_CLASS_S(Powitacq, false, BSDF)
MTS_EXPORT_PLUGIN(Powitacq, "Acquired BRDF");
MTS_NAMESPACE_END
