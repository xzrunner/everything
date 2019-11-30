#include "sop/node/AttributeVOP.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/ParmList.h"

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

    std::vector<sm::vec3> colors;
    for (size_t i = 0, n = src_pts.size(); i < n; ++i)
    {
        // pos
        auto& src_p = src_pts[i];
        m_input->SetPos(src_p->pos);
        auto& dst_p = dst_pts[i];
        auto pos = m_output->GetPos();
        if (pos.IsValid() && dst_p->pos != pos) {
            dst_p->pos = pos;
            dirty = true;
        }

        // color
        auto col = m_output->GetColor();
        if (col.IsValid()) {
            colors.push_back(col);
        }
    }

    if (!colors.empty())
    {
        assert(colors.size() == src_pts.size());
        auto list = std::make_shared<ParmFlt3List>(GEO_ATTR_CD, colors);
        m_geo_impl->GetAttr().AddParmList(GeoAttrClass::Point, list);
        dirty = true;
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

const std::vector<std::shared_ptr<vop::Node>>& AttributeVOP::GetAllChildren() const
{
    return m_eval->GetAllNodes();
}

}
}