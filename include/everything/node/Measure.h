#pragma once

#include "everything/Node.h"
#include "everything/GeoAttribute.h"

namespace evt
{
namespace node
{

class Measure : public Node
{
public:
    enum class Type
    {
        Perimeter,
        Area,
    };

public:
    Measure()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetMesureType(Type type);

    void SetMesureName(const std::string& name);

private:
    void CalcPerimeter(std::string& name, std::vector<VarValue>& val) const;
    void CalcArea(std::string& name, std::vector<VarValue>& val) const;

private:
    Type m_ms_type = Type::Perimeter;

    std::string m_ms_name;

    RTTR_ENABLE(Node)

}; // Measure

}
}