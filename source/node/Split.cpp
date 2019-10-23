#include "sop/node/Split.h"
#include "sop/node/Geometry.h"
#include "sop/node/Blast.h"
#include "sop/node/Output.h"
#include "sop/NodeHelper.h"
#include "sop/Evaluator.h"
#include "sop/GeometryImpl.h"

namespace
{

enum SubNodeIdx
{
    SUB_BLAST_0 = 0,
    SUB_BLAST_1,
    SUB_OUTPUT_0,
    SUB_OUTPUT_1,

    SUB_MAX_COUNT,
};

}

namespace sop
{
namespace node
{

void Split::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_node = NodeHelper::GetInputNode(*this, 0);
    if (!prev_node) {
        return;
    }

    std::static_pointer_cast<Blast>(m_children[SUB_BLAST_0])->
        SetGroupName(m_group_name);
    std::static_pointer_cast<Blast>(m_children[SUB_BLAST_1])->
        SetGroupName(m_group_name);

    Evaluator sub_eval;

    sub_eval.AddNode(prev_node);
    for (auto& c : m_children) {
        sub_eval.AddNode(c);
        c->ClearConnections();
    }

    sub_eval.Connect({ prev_node, 0 }, { m_children[SUB_BLAST_0], 0 });
    sub_eval.Connect({ prev_node, 0 }, { m_children[SUB_BLAST_1], 0 });

    sub_eval.Connect({ m_children[SUB_BLAST_0], 0 }, { m_children[SUB_OUTPUT_0], 0 });
    sub_eval.Connect({ m_children[SUB_BLAST_1], 0 }, { m_children[SUB_OUTPUT_1], 0 });

    sub_eval.Update();

    if (m_children[SUB_OUTPUT_0]) {
        auto out0_geo = m_children[SUB_OUTPUT_0]->GetGeometry();
        if (out0_geo) {
            m_geo_impl = std::make_shared<GeometryImpl>(*out0_geo);
        }
    }
}

void Split::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void Split::InitSubNodes()
{
    m_children.resize(SUB_MAX_COUNT);

    auto blast0 = std::make_shared<Blast>();
    blast0->SetDeleteNonSelected(true);
    m_children[SUB_BLAST_0] = blast0;

    auto output0 = std::make_shared<Output>();
    output0->SetOutputIdx(0);
    m_children[SUB_OUTPUT_0] = output0;

    auto blast1 = std::make_shared<Blast>();
    blast1->SetDeleteNonSelected(false);
    m_children[SUB_BLAST_1] = blast1;

    auto output1 = std::make_shared<Output>();
    output1->SetOutputIdx(1);
    m_children[SUB_OUTPUT_1] = output1;
}

}
}