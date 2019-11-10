#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyNode.h"
#include "sop/PyParmTemplate.h"
#include "sop/NodePropsMgr.h"
#include "sop/Node.h"
#include "sop/Evaluator.h"
#include "sop/node/Geometry.h"

// attribute
#include "sop/node/AttributeCreate.h"
#include "sop/node/AttributeWrangle.h"
#include "sop/node/Sort.h"
#include "sop/node/Measure.h"
// group
#include "sop/node/GroupCreate.h"
#include "sop/node/GroupExpression.h"
#include "sop/node/GroupPromote.h"
// manipulate
#include "sop/node/Delete.h"
#include "sop/node/Transform.h"
// material
#include "sop/node/Color.h"
// nurbs
#include "sop/node/Carve.h"
// polygon
#include "sop/node/Add.h"
#include "sop/node/Boolean.h"
#include "sop/node/Fuse.h"
#include "sop/node/Knife.h"
#include "sop/node/Normal.h"
#include "sop/node/PolyExtrude.h"
#include "sop/node/PolyFrame.h"
// primitive
#include "sop/node/Box.h"
#include "sop/node/Curve.h"
#include "sop/node/Line.h"
#include "sop/node/Primitive.h"
#include "sop/node/Sphere.h"
// utility
#include "sop/node/Blast.h"
#include "sop/node/CopyToPoints.h"
#include "sop/node/Merge.h"
#include "sop/node/ForeachPrimEnd.h"
#include "sop/node/Switch.h"

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

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_create_node_overloads, NodeProxy::CreateNode, 2, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_set_input_overloads, NodeProxy::SetInput, 2, 3)

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
    auto& props = const_cast<NodePropsMgr&>(m_node->GetProps());
    props.SetValue(m_name, Variable(val));
}

void Parm::Set(const std::string& val)
{
    assert(m_node);
    auto& props = const_cast<NodePropsMgr&>(m_node->GetProps());
    props.SetValue(m_name, Variable(val));
}

void ParmTuple::Set(const boost::python::tuple& val)
{
    assert(m_node);
    auto& props = const_cast<NodePropsMgr&>(m_node->GetProps());
    auto var = props.Query(m_name + "x");
    assert(var.type != VarType::Invalid);
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

//////////////////////////////////////////////////////////////////////////
// class ParmTuple
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////
// class NodeProxy
//////////////////////////////////////////////////////////////////////////

NodeProxy::NodeProxy(const NodePtr& node, Evaluator& eval)
    : m_node(node)
    , m_eval(eval)
{
}

std::shared_ptr<NodeProxy>
NodeProxy::CreateNode(const std::string& type, const std::string& name,
                      bool run_init_scripts, bool load_contents, bool exact_type_name)
{
    assert(m_node);
    auto parent = std::dynamic_pointer_cast<node::Geometry>(m_node);
    assert(parent);

    NodePtr child = nullptr;
    // others
    if (type == "geo" || type == "geometry") {
        child = std::make_shared<node::Geometry>();
    }
    // attribute
    else if (type == "attribcreate::2.0") {
        child = std::make_shared<node::AttributeCreate>();
    } else if (type == "attribwrangle") {
        child = std::make_shared<node::AttributeWrangle>();
    } else if (type == "measure") {
        child = std::make_shared<node::Measure>();
    } else if (type == "sort") {
        child = std::make_shared<node::Sort>();
    }
    // group
    else if (type == "groupcreate") {
        child = std::make_shared<node::GroupCreate>();
    } else if (type == "groupexpression") {
        child = std::make_shared<node::GroupExpression>();
    } else if (type == "grouppromote") {
        child = std::make_shared<node::GroupPromote>();
    }
    // manipulate
    else if (type == "delete") {
        child = std::make_shared<node::Delete>();
    } else if (type == "xform") {
        child = std::make_shared<node::Transform>();
    }
    // nurbs
    else if (type == "carve") {
        child = std::make_shared<node::Carve>();
    }
    // polygon
    else if (type == "add") {
        child = std::make_shared<node::Add>();
    } else if (type == "fuse") {
        child = std::make_shared<node::Fuse>();
    } else if (type == "knife") {
        child = std::make_shared<node::Knife>();
    } else if (type == "normal") {
        child = std::make_shared<node::Normal>();
    } else if (type == "polyextrude::2.0") {
        child = std::make_shared<node::PolyExtrude>();
    } else if (type == "polyframe") {
        child = std::make_shared<node::PolyFrame>();
    }
    // primitive
    else if (type == "box") {
        child = std::make_shared<node::Box>();
    } else if (type == "line") {
        child = std::make_shared<node::Line>();
    } else if (type == "primitive") {
        child = std::make_shared<node::Primitive>();
    }
    // utility
    else if (type == "blast") {
        child = std::make_shared<node::Blast>();
    } else if (type == "copytopoints") {
        child = std::make_shared<node::CopyToPoints>();
    } else if (type == "block_begin" || type == "block_end") {
        child = std::make_shared<node::ForeachPrimEnd>();
    } else if (type == "merge") {
        child = std::make_shared<node::Merge>();
    }  else if (type == "switch") {
        child = std::make_shared<node::Switch>();
    }
    // unknown
    else {
        printf("unknown node type %s\n", type.c_str());
        assert(0);
    }
    assert(child);

    m_eval.AddNode(child);

    child->SetName(name);
    node::Geometry::AddChild(parent, child);

    return std::make_shared<NodeProxy>(child, m_eval);
}
std::shared_ptr<NodeProxy> NodeProxy::CreateNetworkBox(const std::string& name)
{
    return std::make_shared<NodeProxy>(nullptr, m_eval);
}
std::shared_ptr<NodeProxy> NodeProxy::FindNetworkBox(const std::string& name)
{
    assert(0);
    return nullptr;
}

std::shared_ptr<NodeProxy> NodeProxy::GetParent()
{
    if (m_node) {
        auto p = m_node->GetParent();
        if (p) {
            return std::make_shared<NodeProxy>(p, m_eval);
        }
    }
    return nullptr;
}
std::shared_ptr<NodeProxy> NodeProxy::GetChild(const std::string& name)
{
    assert(m_node);
    auto parent = std::dynamic_pointer_cast<node::Geometry>(m_node);
    assert(parent);

    auto child = parent->QueryChild(name);
    if (child) {
        return std::make_shared<NodeProxy>(child, m_eval);
    } else {
        return nullptr;
    }
}

void NodeProxy::SetInput(int input_index, const std::shared_ptr<NodeProxy>& item_to_become_input, int output_index)
{
    if (m_node && item_to_become_input->m_node) {
        make_connecting({ item_to_become_input->m_node, output_index }, { m_node, input_index });
    }
}

void BindNode()
{
    // types

    void(Parm::*ParmSet0)(float)              = &Parm::Set;
    void(Parm::*ParmSet1)(const std::string&) = &Parm::Set;
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

    // node
    class_<NodeProxy>("NodeProxy", init<const NodePtr&, Evaluator&>())
        .def("createNode", &NodeProxy::CreateNode, hou_node_create_node_overloads(
            args("type", "name", "run_init_scripts", "load_contents", "exact_type_name")
        ))
        .def("createNetworkBox", &NodeProxy::CreateNetworkBox)
        .def("findNetworkBox", &NodeProxy::FindNetworkBox)
        .def("parent", &NodeProxy::GetParent)
        .def("node", &NodeProxy::GetChild)
        .def("move", &NodeProxy::Move)
        .def("setSelectableInViewport", &NodeProxy::SetSelectableInViewport)
        .def("showOrigin", &NodeProxy::ShowOrigin)
        .def("useXray", &NodeProxy::UseXray)
        .def("hide", &NodeProxy::Hide)
        .def("setSelected", &NodeProxy::SetSelected)
        .def("setParmTemplateGroup", &NodeProxy::SetParmTemplateGroup)
        .def("parmTuple", &NodeProxy::GetParmTuple)
        .def("setExpressionLanguage", &NodeProxy::SetExpressionLanguage)
        .def("setUserData", &NodeProxy::SetUserData)
        .def("bypass", &NodeProxy::Bypass)
        .def("setDisplayFlag", &NodeProxy::SetDisplayFlag)
        .def("setHighlightFlag", &NodeProxy::SetHighlightFlag)
        .def("setHardLocked", &NodeProxy::SetHardLocked)
        .def("setSoftLocked", &NodeProxy::SetSoftLocked)
        .def("setSelectableTemplateFlag", &NodeProxy::SetSelectableTemplateFlag)
        .def("setRenderFlag", &NodeProxy::SetRenderFlag)
        .def("setTemplateFlag", &NodeProxy::SetTemplateFlag)
        .def("setUnloadFlag", &NodeProxy::SetUnloadFlag)
        .def("setInput", &NodeProxy::SetInput, hou_node_set_input_overloads(
            args("input_index", "item_to_become_input", "output_index")
        ))
        .def("setPosition", &NodeProxy::SetPosition)
        .def("setSize", &NodeProxy::SetSize)
        .def("setMinimized", &NodeProxy::SetMinimized)
        .def("setAutoFit", &NodeProxy::SetAutoFit)
        .def("setComment", &NodeProxy::SetComment)
        .def("setColor", &NodeProxy::SetColor)
        .def("parm", &NodeProxy::GetParm)
        .def("indirectInputs", &NodeProxy::IndirectInputs)
    ;

}

}
}