#include "sop/Node.h"

#include <assert.h>

RTTR_REGISTRATION
{

rttr::registration::class_<hdiop::NodePort<sop::NodeVarType>>("sop::Node::Port")
	.property("var", &hdiop::NodePort<sop::NodeVarType>::var)
;

rttr::registration::class_<sop::Node>("sop::Node")
	.method("GetImports", &sop::Node::GetImports)
	.method("GetExports", &sop::Node::GetExports)
;

}

namespace sop
{

Node::Node(size_t build_in_count)
    : m_props(build_in_count)
{
}

void Node::SetParent(const std::shared_ptr<Node>& node)
{
    m_parent = node;

    m_level = node->m_level + 1;
}

}