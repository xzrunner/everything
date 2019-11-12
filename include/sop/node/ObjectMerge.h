#pragma once

#include "sop/Node.h"
#include "sop/typedef.h"

namespace sop
{
namespace node
{

class ObjectMerge : public Node
{
public:
    ObjectMerge()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetObjects(const std::vector<NodePtr>& objs);
    auto& GetObjects() const { return m_objs; }

private:
    std::vector<NodePtr> m_objs;

    RTTR_ENABLE(Node)

}; // ObjectMerge

}
}