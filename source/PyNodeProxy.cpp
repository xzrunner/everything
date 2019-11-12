#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyNodeProxy.h"
#include "sop/Evaluator.h"
#include "sop/node/Geometry.h"

// attribute
#include "sop/node/AttributeCreate.h"
#include "sop/node/AttributePromote.h"
#include "sop/node/AttributeTransfer.h"
#include "sop/node/AttributeWrangle.h"
#include "sop/node/Sort.h"
#include "sop/node/Measure.h"
// group
#include "sop/node/GroupCreate.h"
#include "sop/node/GroupExpression.h"
#include "sop/node/GroupPromote.h"
// import
#include "sop/node/ObjectMerge.h"
// manipulate
#include "sop/node/Delete.h"
#include "sop/node/Peak.h"
#include "sop/node/Transform.h"
// material
#include "sop/node/Color.h"
#include "sop/node/UVTransform.h"
#include "sop/node/UVUnwrap.h"
// nurbs
#include "sop/node/Carve.h"
// polygon
#include "sop/node/Add.h"
#include "sop/node/Boolean.h"
#include "sop/node/Divide.h"
#include "sop/node/Fuse.h"
#include "sop/node/Knife.h"
#include "sop/node/Normal.h"
#include "sop/node/PolyExtrude.h"
#include "sop/node/PolyFill.h"
#include "sop/node/PolyFrame.h"
// primitive
#include "sop/node/Box.h"
#include "sop/node/Curve.h"
#include "sop/node/Line.h"
#include "sop/node/Primitive.h"
#include "sop/node/Sphere.h"
// rigging
#include "sop/node/Lattice.h"
// utility
#include "sop/node/Blast.h"
#include "sop/node/CopyToPoints.h"
#include "sop/node/Merge.h"
#include "sop/node/ForeachPrimEnd.h"
#include "sop/node/Output.h"
#include "sop/node/Python.h"
#include "sop/node/Split.h"
#include "sop/node/Switch.h"

using namespace boost::python;
using namespace sop::py;

namespace
{

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_create_node_overloads, NodeProxy::CreateNode, 2, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_set_input_overloads, NodeProxy::SetInput, 2, 3)

}

namespace sop
{
namespace py
{

Evaluator* NodeProxy::m_eval = nullptr;

NodeProxy::NodeProxy(const NodePtr& node)
    : m_node(node)
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
    }  else if (type == "attribtransfer") {
        child = std::make_shared<node::AttributeTransfer>();
    } else if (type == "attribpromote") {
        child = std::make_shared<node::AttributePromote>();
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
    // import
    else if (type == "object_merge") {
        child = std::make_shared<node::ObjectMerge>();
    }
    // manipulate
    else if (type == "delete") {
        child = std::make_shared<node::Delete>();
    } else if (type == "peak") {
        child = std::make_shared<node::Peak>();
    } else if (type == "xform") {
        child = std::make_shared<node::Transform>();
    }
    // material
    else if (type == "color") {
        child = std::make_shared<node::Color>();
    } else if (type == "uvtransform::2.0") {
        child = std::make_shared<node::UVTransform>();
    } else if (type == "uvunwrap") {
        child = std::make_shared<node::UVUnwrap>();
    }
    // nurbs
    else if (type == "carve") {
        child = std::make_shared<node::Carve>();
    }
    // polygon
    else if (type == "add") {
        child = std::make_shared<node::Add>();
    } else if (type == "boolean::2.0") {
        child = std::make_shared<node::Boolean>();
    } else if (type == "divide") {
        child = std::make_shared<node::Divide>();
    } else if (type == "fuse") {
        child = std::make_shared<node::Fuse>();
    } else if (type == "knife") {
        child = std::make_shared<node::Knife>();
    } else if (type == "normal") {
        child = std::make_shared<node::Normal>();
    } else if (type == "polyextrude::2.0") {
        child = std::make_shared<node::PolyExtrude>();
    } else if (type == "polyfill") {
        child = std::make_shared<node::PolyFill>();
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
    // rigging
    else if (type == "lattice") {
        child = std::make_shared<node::Lattice>();
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
    } else if (type == "output") {
        child = std::make_shared<node::Output>();
    } else if (type == "python") {
        child = std::make_shared<node::Python>();
    } else if (type == "split") {
        child = std::make_shared<node::Split>();
    } else if (type == "switch") {
        child = std::make_shared<node::Switch>();
    }
    // unknown
    else {
        printf("unknown node type %s\n", type.c_str());
        assert(0);
    }
    assert(child);

    assert(m_eval);
    m_eval->AddNode(child);

    child->SetName(name);
    node::Geometry::AddChild(parent, child);

    return std::make_shared<NodeProxy>(child);
}

std::shared_ptr<NodeProxy> NodeProxy::CreateNetworkBox(const std::string& name)
{
    return std::make_shared<NodeProxy>(nullptr);
}

std::shared_ptr<NodeProxy> NodeProxy::FindNetworkBox(const std::string& name)
{
//    assert(0);
    return nullptr;
}

std::shared_ptr<NodeProxy> NodeProxy::GetParent()
{
    if (m_node) {
        auto p = m_node->GetParent();
        if (p) {
            return std::make_shared<NodeProxy>(p);
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
        return std::make_shared<NodeProxy>(child);
    } else {
        return nullptr;
    }
}

void NodeProxy::SetInput(int input_index, const std::shared_ptr<NodeProxy>& item_to_become_input, int output_index)
{
    if (!m_node || !item_to_become_input->m_node) {
        return;
    }

    if (input_index >= m_node->GetImports().size() &&
        m_node->get_type() == rttr::type::get<node::Merge>()) {
        int num = input_index - m_node->GetImports().size() + 1;
        std::static_pointer_cast<node::Merge>(m_node)->AddInputPorts(num);
    }

    make_connecting({ item_to_become_input->m_node, output_index }, { m_node, input_index });
}

void BindNodeProxy()
{
    class_<NodeProxy>("NodeProxy", init<const NodePtr&>())
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