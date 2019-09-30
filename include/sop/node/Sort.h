#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class Sort : public Node
{
public:
    enum class Key
    {
        NoChange,
        X,
        Y,
        Z
    };

public:
    Sort()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetKey(Key key);

private:
    class PointCmp
    {
    public:
        PointCmp(const std::vector<std::shared_ptr<GeoAttribute::Point>>& points, Key key);

        bool operator () (size_t i0, size_t i1) const;

    private:
        const std::vector<std::shared_ptr<GeoAttribute::Point>>& m_points;

        Key m_key;

    }; // PointCmp

private:
    Key m_key = Key::NoChange;

    RTTR_ENABLE(Node)

}; // Sort

}
}