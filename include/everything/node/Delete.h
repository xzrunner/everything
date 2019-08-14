#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class Delete : public Node
{
public:
    enum class EntityType
    {
        Points,
        Edges,
        Face,
    };

public:
    Delete()
    {
        m_imports = {
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

private:
    bool m_delete_non_selected = false;

    EntityType m_entity_type = EntityType::Points;

    std::string m_filter_exp;

    RTTR_ENABLE(Node)

}; // Delete

}
}