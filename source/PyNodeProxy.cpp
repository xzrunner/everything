#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyNodeProxy.h"
#include "sop/Evaluator.h"
#include "sop/node/Subnetwork.h"
#include "sop/node/Merge.h"

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
    auto parent = std::dynamic_pointer_cast<node::Subnetwork>(m_node);
    assert(parent);

    auto child = CreateNodeByName(type);
    assert(child);

    assert(m_eval);
    m_eval->AddNode(child);

    child->SetName(name);
    node::Subnetwork::AddChild(parent, child);

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
    auto parent = std::dynamic_pointer_cast<node::Subnetwork>(m_node);
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

    if (input_index >= static_cast<int>(m_node->GetImports().size()) &&
        m_node->get_type() == rttr::type::get<node::Merge>()) {
        int num = input_index - m_node->GetImports().size() + 1;
        std::static_pointer_cast<node::Merge>(m_node)->AddInputPorts(num);
    }

    auto& in_conns = m_node->GetImports()[input_index].conns;
    if (!in_conns.empty()) {
        const_cast<std::vector<dag::Node<NodeVarType>::PortAddr>&>(in_conns).clear();
    }
    dag::make_connecting<NodeVarType>({ item_to_become_input->m_node, output_index }, { m_node, input_index });
}

NodePtr NodeProxy::CreateNodeByName(const std::string& name) const
{
    std::string node_name;
    if (name == "geometry") {
        node_name = "sop::subnetwork";
    } else {
        node_name = "sop::" + name;
    }
    rttr::type type = rttr::type::get_by_name(node_name);
    if (!type.is_valid()) {
        assert(0);
        return nullptr;
    }

    rttr::variant var = type.create();
    assert(var.is_valid());
    return var.get_value<std::shared_ptr<Node>>();
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