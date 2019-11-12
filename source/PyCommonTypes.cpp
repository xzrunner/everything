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

    void(Parm::*ParmSet0)(float)              = &Parm::Set;
    void(Parm::*ParmSet1)(const std::string&) = &Parm::Set;
    void(Parm::*ParmSet2)(int) = &Parm::Set;
    void(Parm::*ParmSetKeyframe0)(const Keyframe&)       = &Parm::SetKeyframe;
    void(Parm::*ParmSetKeyframe1)(const StringKeyframe&) = &Parm::SetKeyframe;
    class_<Parm>("Parm", init<const std::string&, const NodePtr&>())
        .def("setKeyframe", ParmSetKeyframe0)
        .def("setKeyframe", ParmSetKeyframe1)
        .def("set", ParmSet0)
        .def("set", ParmSet1)
    ;
    class_<ParmTuple>("ParmTuple", init<const std::string&, const NodePtr&>())
        .def("setAutoscope", &ParmTuple::SetAutoscope)
        .def("set", &ParmTuple::Set)
        .def("__getitem__", &ParmTuple::GetItem, return_value_policy<return_by_value>())
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

//////////////////////////////////////////////////////////////////////////
// class Parm
//////////////////////////////////////////////////////////////////////////

void Parm::Set(float val)
{
    assert(m_node);
    auto& props = const_cast<NodePropsMgr&>(m_node->GetProps());
    Variable v(val);
    if (!props.SetValue(m_name, v)) {
        m_node->SetParm(m_name, v);
    }
}

void Parm::Set(const std::string& val)
{
    assert(m_node);
    auto& props = const_cast<NodePropsMgr&>(m_node->GetProps());
    Variable v(val);
    if (!props.SetValue(m_name, v)) {
        m_node->SetParm(m_name, v);
    }
}

void Parm::Set(int val)
{
    assert(m_node);
    auto& props = const_cast<NodePropsMgr&>(m_node->GetProps());
    Variable v(val);
    if (!props.SetValue(m_name, v)) {
        m_node->SetParm(m_name, v);
    }
}

//////////////////////////////////////////////////////////////////////////
// class ParmTuple
//////////////////////////////////////////////////////////////////////////

void ParmTuple::Set(const boost::python::tuple& val)
{
    assert(m_node);
    auto& props = const_cast<NodePropsMgr&>(m_node->GetProps());
    auto var = props.Query(m_name + "x");
    if (var.type != VarType::Invalid)
    {
        switch (var.type)
        {
        case VarType::Float:
            props.SetValue(m_name + "x", Variable(boost::python::extract<float>(val[0])));
            props.SetValue(m_name + "y", Variable(boost::python::extract<float>(val[1])));
            props.SetValue(m_name + "z", Variable(boost::python::extract<float>(val[2])));
            break;
        default:
            assert(0);
        }
    }
    else
    {
        sm::vec3 v3;
        for (size_t i = 0; i < 3; ++i) {
            v3.xyz[i] = boost::python::extract<float>(val[i]);
        }
        m_node->SetParm(m_name, Variable(v3));
    }
}

Parm ParmTuple::GetItem(size_t i)
{
    switch (i)
    {
    case 0:
        return Parm(m_name + "x", m_node);
    case 1:
        return Parm(m_name + "y", m_node);
    case 2:
        return Parm(m_name + "z", m_node);
    default:
        assert(0);
        return Parm("", nullptr);
    }
}

}
}