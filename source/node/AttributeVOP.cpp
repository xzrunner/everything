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

void AttributeVOP::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    if (!m_eval) {
        return;
    }

    std::shared_ptr<vop::node::GeoGlobalParams> input  = nullptr;
    std::shared_ptr<vop::node::GeoOutputVars>   output = nullptr;
    for (auto& node : m_eval->GetAllNodes())
    {
        const auto type = node->get_type();
        if (type == rttr::type::get<vop::node::GeoGlobalParams>()) {
            input = std::static_pointer_cast<vop::node::GeoGlobalParams>(node);
        }
        if (type == rttr::type::get<vop::node::GeoOutputVars>()) {
            output = std::static_pointer_cast<vop::node::GeoOutputVars>(node);
        }
    }
    if (!input || !output) {
        return;
    }

    bool dirty = false;

    auto& src_attr = prev_geo->GetAttr();
    auto& src_pts = src_attr.GetPoints();
    auto& dst_pts = m_geo_impl->GetAttr().GetPoints();
    assert(src_pts.size() == dst_pts.size());

    input->SetPointIdx(0);
    input->SetPrimIdx(0);
    input->SetVertexIdx(0);
    input->SetPointNum(src_pts.size());
    input->SetPrimNum(src_attr.GetPrimtives().size());
    input->SetVertexNum(src_attr.GetVertices().size());

    auto cd_list = src_attr.QueryParmList(GeoAttrClass::Point, GeoAttr::GEO_ATTR_CD);
    auto uv_list = src_attr.QueryParmList(GeoAttrClass::Point, GeoAttr::GEO_ATTR_UV);
    auto n_list  = src_attr.QueryParmList(GeoAttrClass::Point, GeoAttr::GEO_ATTR_NORM);

    std::vector<sm::vec3> colors, normals;
    for (size_t i = 0, n = src_pts.size(); i < n; ++i)
    {
        input->SetPointIdx(i);
        if (cd_list) {
            assert(i < cd_list->Size() && cd_list->GetType() == ParmType::Vector);
            input->SetColor(std::static_pointer_cast<ParmFlt3List>(cd_list)->GetAllItems()[i]);
        }
        if (uv_list) {
            assert(i < uv_list->Size() && uv_list->GetType() == ParmType::Float3);
            input->SetUV(std::static_pointer_cast<ParmFlt3List>(uv_list)->GetAllItems()[i]);
        }
        if (n_list) {
            assert(i < n_list->Size() && n_list->GetType() == ParmType::Vector);
            input->SetNormal(std::static_pointer_cast<ParmFlt3List>(n_list)->GetAllItems()[i]);
        }

        // pos
        auto& src_p = src_pts[i];
        input->SetPos(src_p->pos);
        auto& dst_p = dst_pts[i];
        auto pos = output->GetPos();
        if (pos.IsValid() && dst_p->pos != pos) {
            dst_p->pos = pos;
            dirty = true;
        }

        // color
        auto col = output->GetColor();
        if (col.IsValid()) {
            colors.push_back(col);
        }

        // normal
        auto norm = output->GetNormal();
        if (norm.IsValid()) {
            normals.push_back(norm);
        }
    }

    if (!colors.empty())
    {
        assert(colors.size() == src_pts.size());
        auto list = std::make_shared<ParmFlt3List>(GEO_ATTR_CD, colors);
        m_geo_impl->GetAttr().AddParmList(GeoAttrClass::Point, list);
        dirty = true;
    }

    if (!normals.empty())
    {
        assert(normals.size() == src_pts.size());
        auto list = std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, normals);
        m_geo_impl->GetAttr().AddParmList(GeoAttrClass::Point, list);
        dirty = true;
    }

    if (dirty) {
        m_geo_impl->UpdateByAttr(dev);
    }
}

void AttributeVOP::SetEval(const std::shared_ptr<vop::Evaluator>& eval)
{
    if (m_eval != eval) {
        m_eval = eval;
        SetDirty(true);
    }
}

void AttributeVOP::InitEval()
{
    m_eval = std::make_shared<vop::Evaluator>();

    auto input = std::make_shared<vop::node::GeoGlobalParams>();
    input->SetName("input");
    m_eval->AddNode(input);

    auto output = std::make_shared<vop::node::GeoOutputVars>();
    output->SetName("output");
    m_eval->AddNode(output);
}

}
}