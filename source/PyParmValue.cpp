#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyParmValue.h"
#include "sop/PyCommonTypes.h"
#include "sop/NodeParmsMgr.h"
#include "sop/Node.h"
#include "sop/Widgets.h"

#include "sop/node/Box.h"

using namespace boost::python;
using namespace sop::py;

namespace
{

bool fetch_end_num(const std::string& str, int& out_num, std::string& out_prefix)
{
    if (str.empty()) {
        return false;
    }

    int itr = str.size() - 1;
    int num = 0;
    while (str[itr] >= '0' && str[itr] <= '9' && itr >= 0) {
        num = num * 10 + static_cast<int>(str[itr] - '0');
        --itr;
    }

    if (itr == str.size() - 1) {
        return false;
    } else {
        out_num = num;
        out_prefix = str.substr(0, itr + 1);
        return true;
    }

    return num;
}

bool fetch_end_ramp(const std::string& str, int& out_num,
                    std::string& out_prefix, sop::RampWidgets::COMP_ID& out_comp)
{
    for (size_t i = 0; i < 3; ++i)
    {
        if (str.find_last_of(sop::RampWidgets::COMP_NAMES[i]) != str.size() - 1) {
            continue;
        }

        auto rem = str.substr(0, str.size() - strlen(sop::RampWidgets::COMP_NAMES[i]));
        if (!fetch_end_num(rem, out_num, out_prefix)) {
            continue;
        }

        if (out_prefix.find_last_of("ramp") != out_prefix.size() - 1) {
            continue;
        }

        out_prefix = out_prefix.substr(0, out_prefix.size() - strlen("ramp"));
        out_comp = static_cast<sop::RampWidgets::COMP_ID>(i);

        return true;
    }

    return false;
}

bool parm_set_val(sop::NodeParmsMgr& parms, const std::string& key, const sop::Variable& val)
{
    if (parms.IsExist(key))
    {
        parms.SetValue(key, val);
        return true;
    }

    int idx;
    std::string name;
    sop::RampWidgets::COMP_ID ramp_comp;
    if (fetch_end_num(key, idx, name))
    {
        if (parms.IsExist(name)) {
            parms.SetArrayValue(name, idx, val);
            return true;
        } else {
            return false;
        }
    }
    else if (fetch_end_ramp(key, idx, name, ramp_comp))
    {
        if (parms.IsExist(name)) {
            parms.SetArrayValue(name, idx, val, sop::RampWidgets::COMP_NAMES[ramp_comp]);
            return true;
        } else {
            return false;
        }
    }

    return false;
}

}

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
    if (!parm_set_val(parms, m_name, Variable(val))) {
        auto type = m_node->get_type().get_name().to_string();
        printf("parm_set_float %s, %s\n", type.c_str(), m_name.c_str());
    }
}

void Parm::Set(const std::string& val)
{
    assert(m_node);
    auto& parms = const_cast<NodeParmsMgr&>(m_node->GetParms());
    if (!parm_set_val(parms, m_name, Variable(val))) {
        auto type = m_node->get_type().get_name().to_string();
        printf("parm_set_str %s, %s\n", type.c_str(), m_name.c_str());
    }
}

void Parm::Set(int val)
{
    assert(m_node);
    auto& parms = const_cast<NodeParmsMgr&>(m_node->GetParms());
    if (!parm_set_val(parms, m_name, Variable(val))) {
        auto type = m_node->get_type().get_name().to_string();
        printf("parm_set_int %s, %s\n", type.c_str(), m_name.c_str());
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

    if (!parm_set_val(parms, m_name, Variable(v3))) {
        auto type = m_node->get_type().get_name().to_string();
        printf("parm_set_tuple %s, %s\n", type.c_str(), m_name.c_str());
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