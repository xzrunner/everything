#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyCommonTypes.h"
#include "sop/PyParmTemplate.h"
#include "sop/NodePropsMgr.h"
#include "sop/Node.h"

#include <SM_Vector.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace sop::py;

namespace
{

ParmTemplateGroup hou_create_parm_template_group()
{
    return ParmTemplateGroup();
}

Color hou_create_color(const list& rgb)
{
    Color col;
    col.r = boost::python::extract<float>(rgb[0]);
    col.g = boost::python::extract<float>(rgb[1]);
    col.b = boost::python::extract<float>(rgb[2]);
    return col;
}

Keyframe hou_create_keyframe()
{
    return Keyframe();
}

StringKeyframe hou_create_stringkeyframe()
{
    return StringKeyframe();
}

sm::vec2 hou_create_vec2(float x, float y)
{
    return sm::vec2(x, y);
}

}

namespace sop
{
namespace py
{

void BindCommonTypes()
{
    enum_<ExprLanguage>("exprLanguage")
        .value("Python", ExprLanguage::Python)
        .value("Hscript", ExprLanguage::Hscript)
    ;
    enum_<ScriptLanguage>("scriptLanguage")
        .value("Python", ScriptLanguage::Python)
        .value("Hscript", ScriptLanguage::Hscript)
    ;

    class_<sm::vec2>("Vector2", init<float, float>());
    def("Vector2", hou_create_vec2);

    class_<ParmTemplateGroup>("ParmTemplateGroup")
        .def("append", &ParmTemplateGroup::Append)
    ;
    def("ParmTemplateGroup", hou_create_parm_template_group);

    class_<Color>("Color");
    def("Color", hou_create_color);

    class_<Keyframe>("Keyframe")
        .def("setTime", &Keyframe::SetTime)
        .def("setExpression", &Keyframe::SetExpression)
        .def("interpretAccelAsRatio", &Keyframe::InterpretAccelAsRatio)
    ;
    def("Keyframe", hou_create_keyframe);

    class_<StringKeyframe>("StringKeyframe")
        .def("setTime", &StringKeyframe::SetTime)
        .def("setExpression", &StringKeyframe::SetExpression)
    ;
    def("StringKeyframe", hou_create_stringkeyframe);
}

}
}