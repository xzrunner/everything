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
        Z,
        Shift,
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

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override;

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

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Sort.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Sort

}
}