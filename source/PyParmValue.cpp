#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyParmValue.h"
#include "sop/PyCommonTypes.h"
#include "sop/NodeParmsMgr.h"
#include "sop/Node.h"

using namespace boost::python;
using namespace sop::py;

namespace sop
{
namespace py
{

//////////////////////////////////////////////////////////////////////////
// class Parm
//////////////////////////////////////////////////////////////////////////

void Parm::Set(float val)
{
    assert(m_node);
    auto& parms = const_cast<NodeParmsMgr&>(m_node->GetParms());
    Variable v(val);
    if (!parms.SetValue(m_name, v, true)) {
        m_node->SetParm(m_name, v);
    }
}

void Parm::Set(const std::string& val)
{
    assert(m_node);
    auto& parms = const_cast<NodeParmsMgr&>(m_node->GetParms());
    Variable v(val);
    if (!parms.SetValue(m_name, v, true)) {
        m_node->SetParm(m_name, v);
    }
}

void Parm::Set(int val)
{
    assert(m_node);
    auto& parms = const_cast<NodeParmsMgr&>(m_node->GetParms());
    Variable v(val);
    if (!parms.SetValue(m_name, v, true)) {
        m_node->SetParm(m_name, v);
    }
}

//////////////////////////////////////////////////////////////////////////
// class ParmTuple
//////////////////////////////////////////////////////////////////////////

void ParmTuple::Set(const boost::python::tuple& val)
{
    assert(m_node);
    auto& parms = const_cast<NodeParmsMgr&>(m_node->GetParms());

    sm::vec3 v3;
    for (size_t i = 0; i < 3; ++i) {
        v3.xyz[i] = boost::python::extract<float>(val[i]);
    }
    Variable v(v3);
    if (!parms.SetValue(m_name, v)) {
        m_node->SetParm(m_name, v);
    }

    if (!parms.SetValue(m_name, v, true)) {
        m_node->SetParm(m_name, v);
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

void BindParmValue()
{
    void(Parm::*ParmSet0)(float)              = &Parm::Set;
    void(Parm::*ParmSet1)(const std::string&) = &Parm::Set;
    void(Parm::*ParmSet2)(int)                = &Parm::Set;
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
}

}
}