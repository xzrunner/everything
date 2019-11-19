#include "sop/node/AttributeVOP.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

#include <vop/Evaluator.h>
#include <vop/node/GeoGlobalParams.h>
#include <vop/node/GeoOutputVars.h>

namespace sop
{
namespace node
{

void AttributeVOP::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    bool dirty = false;

    auto& src_pts = prev_geo->GetAttr().GetPoints();
    auto& dst_pts = m_geo_impl->GetAttr().GetPoints();
    assert(src_pts.size() == dst_pts.size());
    for (size_t i = 0, n = src_pts.size(); i < n; ++i)
    {
        auto& src_p = src_pts[i];
        m_input->SetPos(src_p->pos);
        auto& dst_p = dst_pts[i];
        auto pos = m_output->GetPos();
        if (pos.IsValid() && dst_p->pos != pos) {
            dst_p->pos = pos;
            dirty = true;
        }
    }

    if (dirty) {
        m_geo_impl->UpdateByAttr();
    }
}

void AttributeVOP::InitEval()
{
    m_eval = std::make_shared<vop::Evaluator>();

    m_input  = std::make_shared<vop::node::GeoGlobalParams>();
    m_output = std::make_shared<vop::node::GeoOutputVars>();
    m_eval->AddNode(m_input);
    m_eval->AddNode(m_output);
}

}
}