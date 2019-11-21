#include "sop/VexFuncs.h"
#include "sop/EvalContext.h"
#include "sop/GeometryImpl.h"
#include "sop/Evaluator.h"
#include "sop/node/Geometry.h"

#include <vexc/EvalAST.h>
#include <vexc/StringPool.h>
#include <cpputil/StringHelper.h>

namespace
{

vexc::Variant ToVexcVar(const sop::EvalContext& ctx, const sop::Variable& var, int component = -1)
{
    switch (var.type)
    {
    case sop::VarType::Invalid:
        return vexc::Variant();
    case sop::VarType::Bool:
        return vexc::Variant(var.b);
    case sop::VarType::Int:
        return vexc::Variant(var.i);
    case sop::VarType::Float:
        return vexc::Variant(var.f);
    case sop::VarType::Float3:
    {
        if (component != -1)
        {
            assert(component >= 0 && component < 3);
            auto v3 = static_cast<const sm::vec3*>(var.p);
            return vexc::Variant(v3->xyz[component]);
        }
        else
        {
            auto v3 = static_cast<const sm::vec3*>(var.p);
            auto new_v3 = ctx.var_buf.Clone(*v3);
            return vexc::Variant(vexc::VarType::Float3, (void*)(new_v3->xyz));
        }
    }
    case sop::VarType::Double:
        return vexc::Variant(var.d);
    //case VarType::String:
    //{
    //    auto str = static_cast<const char*>(v.p);
    //    auto buf = vexc::StringPool::InsertAndQuery(str, strlen(str));
    //    return vexc::Variant(vexc::VarType::String, buf);
    //}
    default:
        assert(0);
        return vexc::Variant();
    }
}

// ch* funcs
vexc::Variant eval_channel(const std::vector<vexc::Variant>& params, const void* ud)
{
    if (params.empty()) {
        return vexc::Variant();
    }

    auto& p = params[0];
    if (p.type != vexc::VarType::String) {
        return vexc::Variant();
    }

    auto ctx = static_cast<const sop::EvalContext*>(ud);

    std::string path(vexc::StringPool::VoidToString(p.p));
    std::vector<std::string> tokens;
    cpputil::StringHelper::Split(path, "/", tokens);
    auto curr_node = const_cast<sop::Node*>(ctx->node);
    int curr_level = curr_node->GetLevel();
    const int begin_level = curr_level;
    for (size_t i = 0, n = tokens.size(); i < n; ++i)
    {
        if (!curr_node && curr_level == begin_level - 1) {
            curr_node = ctx->eval->QueryNodeByName(tokens[i]).get();
            if (curr_node) {
                continue;
            }
        }

        if (!curr_node) {
            break;
        }

        auto& t = tokens[i];
        if (t == "..") {
            curr_node = curr_node->GetParent().get();
            --curr_level;
            continue;
        }

        // query child
        assert(curr_node);
        if (curr_node->get_type() == rttr::type::get<sop::node::Geometry>())
        {
            auto child = static_cast<const sop::node::Geometry*>(curr_node)->QueryChild(t);
            if (child) {
                curr_node = child.get();
                continue;
            }
        }

        // query prop
        assert(curr_node);
        auto var = curr_node->GetProps().Query(t);
        if (var.type == sop::VarType::Invalid) {
            var = curr_node->GetParms().Query(t);
        }
        return ToVexcVar(*ctx, var);
    }

    return vexc::Variant();
}

}

namespace sop
{

void SetupVexFuncs()
{
    // ATTRIBUTES AND INTRINSICS

    vexc::RegistBuildInFunc("point", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.size() < 4) {
            return vexc::Variant();
        }
        for (int i = 0; i < 4; ++i) {
            if (params[i].type == vexc::VarType::Invalid) {
                return vexc::Variant();
            }
        }

        auto ctx = static_cast<const EvalContext*>(ud);
        Node* base_node = const_cast<Node*>(ctx->node);

        assert(params[0].type == vexc::VarType::String);
        std::string path(vexc::StringPool::VoidToString(params[0].p));
        auto surface_node = ctx->eval->QueryNodeByPath(base_node, path);
        if (!surface_node) {
            return vexc::Variant();
        }

        auto geo = surface_node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }

        assert(params[1].type == vexc::VarType::Int);
        int point_num = params[1].i;

        assert(params[2].type == vexc::VarType::String);
        std::string attrib_name = vexc::StringPool::VoidToString(params[2].p);

        assert(params[3].type == vexc::VarType::Int);
        int component = params[3].i;

        auto var = geo->GetAttr().QueryAttr(GeoAttrClass::Point, attrib_name, point_num);
        return ToVexcVar(*ctx, var, component);
    });

    vexc::RegistBuildInFunc("prim", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.size() < 4) {
            return vexc::Variant();
        }
        for (int i = 0; i < 4; ++i) {
            if (params[i].type == vexc::VarType::Invalid) {
                return vexc::Variant();
            }
        }

        auto ctx = static_cast<const EvalContext*>(ud);
        Node* base_node = const_cast<Node*>(ctx->node);

        assert(params[0].type == vexc::VarType::String);
        std::string path(vexc::StringPool::VoidToString(params[0].p));
        auto surface_node = ctx->eval->QueryNodeByPath(base_node, path);
        if (!surface_node) {
            return vexc::Variant();
        }

        auto geo = surface_node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }

        assert(params[1].type == vexc::VarType::Int);
        int prim_num = params[1].i;

        assert(params[2].type == vexc::VarType::String);
        std::string attrib_name = vexc::StringPool::VoidToString(params[2].p);

        assert(params[3].type == vexc::VarType::Int);
        int component = params[3].i;

        auto var = geo->GetAttr().QueryAttr(GeoAttrClass::Primitive, attrib_name, prim_num);
        return ToVexcVar(*ctx, var, component);
    });

    vexc::RegistBuildInFunc("setattrib", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.size() < 6) {
            return vexc::Variant();
        }

        auto geohandle   = params[0].ToInt(ud);
        auto attr_class  = vexc::StringPool::VoidToString(params[1].p);
        auto attr_name   = vexc::StringPool::VoidToString(params[2].p);
        auto element_num = params[3].ToInt(ud);
        auto vertex_num  = params[4].ToInt(ud);
        auto value       = params[5].ToFloat(ud);

        assert(vertex_num == 0);

        assert(geohandle == 0);
        auto ctx = static_cast<const EvalContext*>(ud);
        if (!ctx->node) {
            return vexc::Variant();
        }
        auto geo = ctx->node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }

        GeoAttrClass cls;
        if (attr_class == "point") {
            cls = GeoAttrClass::Point;
        } else if (attr_class == "vertex") {
            cls = GeoAttrClass::Vertex;
        } else if (attr_class == "detail" || attr_class == "global") {
            cls = GeoAttrClass::Detail;
        } else {
            return vexc::Variant();
        }

        auto& attr = geo->GetAttr();

        auto& desc = attr.GetAttrDesc(cls);
        int attr_idx = -1;
        for (int i = 0, n = desc.size(); i < n; ++i) {
            if (desc[i].GetName() == attr_name) {
                attr_idx = i;
                break;
            }
        }

        if (attr_idx == -1)
        {
            attr_idx = desc.size();

            auto new_desc = desc;
            new_desc.push_back({ attr_name, GeoAttrType::Float });
            const_cast<GeoAttribute&>(attr).SetAttrDesc(cls, new_desc);

            switch (cls)
            {
            case GeoAttrClass::Point:
                for (auto& p : attr.GetPoints()) {
                    p->vars.push_back(VarValue());
                }
                break;
            case GeoAttrClass::Vertex:
                for (auto& v : attr.GetVertices()) {
                    v->vars.push_back(VarValue());
                }
                break;
            case GeoAttrClass::Primitive:
                for (auto& prim : attr.GetPrimtives()) {
                    prim->vars.push_back(VarValue());
                }
                break;
            case GeoAttrClass::Detail:
            {
                auto& detail = const_cast<GeoAttribute::Detail&>(attr.GetDetail());
                detail.vars.push_back(VarValue());
            }
                break;
            default:
                assert(0);
            }
        }

        switch (cls)
        {
        case GeoAttrClass::Point:
        {
            auto& pts = attr.GetPoints();
            assert(element_num >= 0 && element_num < static_cast<int>(pts.size()));
            pts[element_num]->vars[attr_idx].f = value;
        }
            break;
        case GeoAttrClass::Vertex:
        {
            auto& vts = attr.GetVertices();
            assert(element_num >= 0 && element_num < static_cast<int>(vts.size()));
            vts[element_num]->vars[attr_idx].f = value;
        }
            break;
        case GeoAttrClass::Primitive:
        {
            auto& prims = attr.GetPrimtives();
            assert(element_num >= 0 && element_num < static_cast<int>(prims.size()));
            prims[element_num]->vars[attr_idx].f = value;
        }
            break;
        case GeoAttrClass::Detail:
        {
            auto& detail = const_cast<GeoAttribute::Detail&>(attr.GetDetail());
            detail.vars[attr_idx].f = value;
        }
            break;
        }

        return vexc::Variant();
    });

    // INTERPOLATION

    vexc::RegistBuildInFunc("fit", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.size() < 5) {
            return vexc::Variant();
        }
        for (int i = 0; i < 5; ++i) {
            if (params[i].type == vexc::VarType::Invalid) {
                return vexc::Variant();
            }
        }

        auto num     = params[0].ToFloat(ud);
        auto old_min = params[1].ToFloat(ud);
        auto old_max = params[2].ToFloat(ud);
        auto new_min = params[3].ToFloat(ud);
        auto new_max = params[4].ToFloat(ud);

        if (old_min == old_max) {
            return params[0];
        } else {
            return vexc::Variant(new_min + (num - old_min) / (old_max - old_min) * (new_max - new_min));
        }
    });

    // MEASURE

    vexc::RegistBuildInFunc("getbbox_center", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.empty()) {
            auto& p = params[0];
            if (p.type != vexc::VarType::Int || p.i != 0) {
                return vexc::Variant();
            }
        }

        auto ctx = static_cast<const EvalContext*>(ud);
        if (!ctx->node) {
            return vexc::Variant();
        }
        auto geo = ctx->node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }

        auto& aabb = geo->GetAttr().GetAABB();
        auto v = ctx->var_buf.Clone(aabb.Center());
        return vexc::Variant(vexc::VarType::Float3, (void*)(v->xyz));
    });

    vexc::RegistBuildInFunc("getbbox_max", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.empty()) {
            auto& p = params[0];
            if (p.type != vexc::VarType::Int || p.i != 0) {
                return vexc::Variant();
            }
        }

        auto ctx = static_cast<const EvalContext*>(ud);
        if (!ctx->node) {
            return vexc::Variant();
        }
        auto geo = ctx->node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }

        auto& aabb = geo->GetAttr().GetAABB();
        auto v = ctx->var_buf.Clone(sm::vec3(aabb.max));
        return vexc::Variant(vexc::VarType::Float3, (void*)(v->xyz));
    });

    vexc::RegistBuildInFunc("getbbox_min", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.empty()) {
            auto& p = params[0];
            if (p.type != vexc::VarType::Int || p.i != 0) {
                return vexc::Variant();
            }
        }

        auto ctx = static_cast<const EvalContext*>(ud);
        if (!ctx->node) {
            return vexc::Variant();
        }
        auto geo = ctx->node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }

        auto& aabb = geo->GetAttr().GetAABB();
        auto v = ctx->var_buf.Clone(sm::vec3(aabb.min));
        return vexc::Variant(vexc::VarType::Float3, (void*)(v->xyz));
    });

    vexc::RegistBuildInFunc("getbbox_size", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.empty()) {
            auto& p = params[0];
            if (p.type != vexc::VarType::Int || p.i != 0) {
                return vexc::Variant();
            }
        }

        auto ctx = static_cast<const EvalContext*>(ud);
        if (!ctx->node) {
            return vexc::Variant();
        }
        auto geo = ctx->node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }

        auto& aabb = geo->GetAttr().GetAABB();
        auto v = ctx->var_buf.Clone(aabb.Size());
        return vexc::Variant(vexc::VarType::Float3, (void*)(v->xyz));
    });

    // NODES

    vexc::RegistBuildInFunc("ch", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant {
        return eval_channel(params, ud);
    });

    vexc::RegistBuildInFunc("chs", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant {
        return eval_channel(params, ud);
    });

    // GETTER
    vexc::RegistGetter([](const char* sym, const void* ud)->vexc::Variant
    {
        assert(strlen(sym) > 0);

        if (strcmp(sym, "@P") == 0)
        {
            auto ctx = static_cast<const EvalContext*>(ud);
            if (!ctx->node) {
                return vexc::Variant();
            }
            auto geo = ctx->node->GetGeometry();
            if (!geo) {
                return vexc::Variant();
            }
            if (ctx->attr_type != GeoAttrClass::Point) {
                return vexc::Variant();
            }

            const auto& points = geo->GetAttr().GetPoints();
            if (ctx->attr_idx < 0 || ctx->attr_idx >= static_cast<int>(points.size())) {
                return vexc::Variant();
            }

            auto v = ctx->var_buf.Clone(points[ctx->attr_idx]->pos);
            return vexc::Variant(vexc::VarType::Float3, (void*)(v->xyz));
        }
        else if (strcmp(sym, "@N") == 0 || strcmp(sym, "v@N") == 0)
        {
            auto ctx = static_cast<const EvalContext*>(ud);
            if (!ctx->node) {
                return vexc::Variant();
            }
            auto geo = ctx->node->GetGeometry();
            if (!geo) {
                return vexc::Variant();
            }
            if (ctx->attr_type != GeoAttrClass::Point) {
                return vexc::Variant();
            }

            auto& attr = geo->GetAttr();
            const auto& points = attr.GetPoints();
            if (ctx->attr_idx < 0 || ctx->attr_idx >= static_cast<int>(points.size())) {
                return vexc::Variant();
            }

            auto norm_idx = attr.QueryAttrIdx(GeoAttrClass::Point, GEO_ATTR_NORM);
            if (norm_idx < 0) {
                return vexc::Variant();
            }

            auto v3 = static_cast<const sm::vec3*>(points[ctx->attr_idx]->vars[norm_idx].p);
            auto new_v3 = ctx->var_buf.Clone(*v3);
            return vexc::Variant(vexc::VarType::Float3, (void*)(new_v3->xyz));
        }
        else if (strcmp(sym, "@ptnum") == 0)
        {
            auto ctx = static_cast<const EvalContext*>(ud);
            if (ctx->attr_type != GeoAttrClass::Point) {
                return vexc::Variant();
            }
            return vexc::Variant(ctx->attr_idx);
        }
        // find from attrs
        else if (sym[0] == '@')
        {
            auto ctx = static_cast<const EvalContext*>(ud);
            if (!ctx->node) {
                return vexc::Variant();
            }
            auto geo = ctx->node->GetGeometry();
            if (!geo) {
                return vexc::Variant();
            }

            auto attr_name = std::string(&sym[1]);
            auto var = geo->GetAttr().QueryAttr(ctx->attr_type, attr_name, ctx->attr_idx);
            return ToVexcVar(*ctx, var);
        }
        else if (strcmp(sym, "$SIZEX") == 0)
        {
            auto ctx = static_cast<const EvalContext*>(ud);
            if (!ctx->node) {
                return vexc::Variant();
            }
            auto geo = ctx->node->GetGeometry();
            if (geo) {
                return vexc::Variant(geo->GetAttr().GetAABB().Width());
            } else {
                return vexc::Variant();
            }
        }
        else if (strcmp(sym, "$SIZEY") == 0)
        {
            auto ctx = static_cast<const EvalContext*>(ud);
            if (!ctx->node) {
                return vexc::Variant();
            }
            auto geo = ctx->node->GetGeometry();
            if (geo) {
                return vexc::Variant(geo->GetAttr().GetAABB().Height());
            } else {
                return vexc::Variant();
            }
        }
        else if (strcmp(sym, "$SIZEZ") == 0)
        {
            auto ctx = static_cast<const EvalContext*>(ud);
            if (!ctx->node) {
                return vexc::Variant();
            }
            auto geo = ctx->node->GetGeometry();
            if (geo) {
                return vexc::Variant(geo->GetAttr().GetAABB().Depth());
            } else {
                return vexc::Variant();
            }
        }
        else
        {
            assert(0);
        }

        return vexc::Variant();
    });

    // SETTER
    vexc::RegistSetter([](const char* sym, vexc::Variant var, const void* ud)
    {
        if (strcmp(sym, "@up") == 0 || strcmp(sym, "v@up") == 0)
        {
            if (var.type != vexc::VarType::Float3) {
                return;
            }

            auto ctx = static_cast<const EvalContext*>(ud);
            if (!ctx->node) {
                return;
            }
            auto geo = ctx->node->GetGeometry();
            if (!geo) {
                return;
            }
            if (ctx->attr_type != GeoAttrClass::Point) {
                return;
            }

            auto& attr = geo->GetAttr();
            const auto& points = attr.GetPoints();
            if (ctx->attr_idx < 0 || ctx->attr_idx >= static_cast<int>(points.size())) {
                return;
            }

            auto val_r = static_cast<const sm::vec3*>(var.p);
            auto up_idx = attr.QueryAttrIdx(GeoAttrClass::Point, GEO_ATTR_UP);
            if (up_idx >= 0)
            {
                auto val_l = static_cast<const sm::vec3*>(points[ctx->attr_idx]->vars[up_idx].p);
                const_cast<sm::vec3&>(*val_l) = *val_r;
            }
            else
            {
                std::vector<VarValue> vars(points.size(), VarValue(sm::vec3(0, 0, 0)));
                vars[ctx->attr_idx] = VarValue(*val_r);
                attr.AddAttr(GeoAttrClass::Point, GEO_ATTR_UP, vars);
            }
        }
        else
        {
            assert(0);
        }
    });
}

}