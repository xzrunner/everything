#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

#include <SM_Vector.h>
#include <halfedge/TopoID.h>

namespace sop
{
namespace node
{

class Add : public Node
{
public:
    Add()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

    void SetStdSwitcher1(int sel);
    int GetStdSwitcher1() { return m_stdswitcher1; }

    void SetSwitcher1(int sel);
    int GetSwitcher1() { return m_switcher1; }

    void SetPointsNum(size_t num);
    size_t GetPointsNum() const;

private:
    std::vector<std::pair<he::TopoID, sm::vec3>>
        GetPrevVertices() const;

private:
    int m_stdswitcher1 = 1;
    int m_switcher1 = 1;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Add.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Add

}
}