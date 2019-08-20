#pragma once

#include "everything/Node.h"
#include "everything/GeoAttribute.h"

namespace evt
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

    virtual void Execute(TreeContext& ctx) override;

    void SetKey(Key key);

private:
    class PointCmp
    {
    public:
        PointCmp(Key key);

        bool operator () (const std::shared_ptr<GeoAttribute::Point>& i0,
            const std::shared_ptr<GeoAttribute::Point>& i1) const;

    private:
        Key m_key;

    }; // PointCmp

private:
    Key m_key = Key::NoChange;

    RTTR_ENABLE(Node)

}; // Sort

}
}