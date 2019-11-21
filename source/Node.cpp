#include "sop/Node.h"

#include <assert.h>

namespace sop
{

Node::Node(size_t build_in_count)
    : m_props(build_in_count)
    , m_parms(*this)
{
}

void Node::SetParent(const std::shared_ptr<Node>& node)
{
    m_parent = node;

    m_level = node->m_level + 1;
}

}