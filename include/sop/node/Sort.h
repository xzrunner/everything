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

    enum PropID
    {
        POINT_OFFSET,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "pointoffset"
    };

public:
    Sort()
        : Node(MAX_BUILD_IN_PROP)
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };

        InitProps();
    }

    virtual void Execute(Evaluator& eval) override;

    void SetKey(Key key);

    void SetPointReverse(bool reverse);

    void SetPointOffset(int off);

private:
    void InitProps();

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

    bool m_point_reverse = false;

    RTTR_ENABLE(Node)

}; // Sort

}
}