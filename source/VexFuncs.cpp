#include "sop/VexFuncs.h"
#include "sop/EvalContext.h"
#include "sop/GeometryImpl.h"
#include "sop/Evaluator.h"
#include "sop/node/Geometry.h"

#include <vexc/EvalAST.h>
#include <vexc/StringPool.h>
#include <cpputil/StringHelper.h>

namespace sop
{

void SetupVexFuncs()
{
    // ATTRIBUTES AND INTRINSICS

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
        sop::Node* base_node = const_cast<sop::Node*>(ctx->node);

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
        int attrib_index = params[3].i;

        auto var = geo->GetAttr().QueryAttr(GeoAttrClass::Primitive, attrib_name, attrib_index);
        if (var.type == VarType::Invalid) {
            return vexc::Variant();
        }
        switch (var.type)
        {
        case VarType::Bool:
            return vexc::Variant(var.b);
        case VarType::Int:
            return vexc::Variant(var.i);
        case VarType::Float:
            return vexc::Variant(var.f);
        case VarType::Float3:
        {
            auto buf = new float[3];
            memcpy(buf, var.p, sizeof(buf));
            assert(attrib_index >= 0 && attrib_index < 3);
            vexc::Variant ret(buf[attrib_index]);
            delete[] buf;
            return ret;
        }
        case VarType::Double:
            return vexc::Variant(var.d);
        default:
            assert(0);
            return vexc::Variant();
        }
    });

    vexc::RegistBuildInFunc("setattrib", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.size() < 6) {
            return vexc::Variant();
        }

        auto geohandle   = params[0].ToInt();
        auto attr_class  = vexc::StringPool::VoidToString(params[1].p);
        auto attr_name   = vexc::StringPool::VoidToString(params[2].p);
        auto element_num = params[3].ToInt();
        auto vertex_num  = params[4].ToInt();
        auto value       = params[5].ToFloat();

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

        sop::GeoAttrClass cls;
        if (attr_class == "point") {
            cls = sop::GeoAttrClass::Point;
        } else if (attr_class == "vertex") {
            cls = sop::GeoAttrClass::Vertex;
        } else if (attr_class == "detail" || attr_class == "global") {
            cls = sop::GeoAttrClass::Detail;
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
            new_desc.push_back({ attr_name, sop::GeoAttrType::Float });
            const_cast<sop::GeoAttribute&>(attr).SetAttrDesc(cls, new_desc);

            switch (cls)
            {
            case sop::GeoAttrClass::Point:
                for (auto& p : attr.GetPoints()) {
                    p->vars.push_back(sop::VarValue());
                }
                break;
            case sop::GeoAttrClass::Vertex:
                for (auto& v : attr.GetVertices()) {
                    v->vars.push_back(sop::VarValue());
                }
                break;
            case sop::GeoAttrClass::Primitive:
                for (auto& prim : attr.GetPrimtives()) {
                    prim->vars.push_back(sop::VarValue());
                }
                break;
            case sop::GeoAttrClass::Detail:
            {
                auto& detail = const_cast<sop::GeoAttribute::Detail&>(attr.GetDetail());
                detail.vars.push_back(sop::VarValue());
            }
                break;
            default:
                assert(0);
            }
        }

        switch (cls)
        {
        case sop::GeoAttrClass::Point:
        {
            auto& pts = attr.GetPoints();
            assert(element_num >= 0 && element_num < static_cast<int>(pts.size()));
            pts[element_num]->vars[attr_idx].f = value;
        }
            break;
        case sop::GeoAttrClass::Vertex:
        {
            auto& vts = attr.GetVertices();
            assert(element_num >= 0 && element_num < static_cast<int>(vts.size()));
            vts[element_num]->vars[attr_idx].f = value;
        }
            break;
        case sop::GeoAttrClass::Primitive:
        {
            auto& prims = attr.GetPrimtives();
            assert(element_num >= 0 && element_num < static_cast<int>(prims.size()));
            prims[element_num]->vars[attr_idx].f = value;
        }
            break;
        case sop::GeoAttrClass::Detail:
        {
            auto& detail = const_cast<sop::GeoAttribute::Detail&>(attr.GetDetail());
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

        auto num     = params[0].ToFloat();
        auto old_min = params[1].ToFloat();
        auto old_max = params[2].ToFloat();
        auto new_min = params[3].ToFloat();
        auto new_max = params[4].ToFloat();

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
        auto v = new sm::vec3(aabb.Center());
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
        auto v = new sm::vec3(aabb.max);
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
        auto v = new sm::vec3(aabb.min);
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
        auto v = new sm::vec3(aabb.Size());
        return vexc::Variant(vexc::VarType::Float3, (void*)(v->xyz));
    });

    // NODES

    vexc::RegistBuildInFunc("ch", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.empty()) {
            return vexc::Variant();
        }

        auto& p = params[0];
        if (p.type != vexc::VarType::String) {
            return vexc::Variant();
        }

        auto ctx = static_cast<const EvalContext*>(ud);

        std::string path(vexc::StringPool::VoidToString(p.p));
        std::vector<std::string> tokens;
        cpputil::StringHelper::Split(path, "/", tokens);
        sop::Node* curr_node = const_cast<sop::Node*>(ctx->node);
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
            if (curr_node->get_type() == rttr::type::get<node::Geometry>())
            {
                auto child = static_cast<const node::Geometry*>(curr_node)->QueryChild(t);
                if (child) {
                    curr_node = child.get();
                    continue;
                }
            }

            // query prop
            assert(curr_node);
            auto v = curr_node->GetProps().Query(t);
            switch (v.type)
            {
            case VarType::Invalid:
                return vexc::Variant();
            case VarType::Bool:
                return vexc::Variant(v.b);
            case VarType::Int:
                return vexc::Variant(v.i);
            case VarType::Float:
                return vexc::Variant(v.f);
            case VarType::Float3:
            {
                auto buf = new float[3];
                memcpy(buf, v.p, sizeof(buf));
                return vexc::Variant(vexc::VarType::Float3, buf);
            }
            case VarType::Double:
                return vexc::Variant(v.d);
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

        return vexc::Variant();
    });

    // ATTRIBUTE

    vexc::RegistBuildInFunc("@P", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        auto ctx = static_cast<const EvalContext*>(ud);
        if (!ctx->node) {
            return vexc::Variant();
        }
        auto geo = ctx->node->GetGeometry();
        if (!geo) {
            return vexc::Variant();
        }
        const auto& points = geo->GetAttr().GetPoints();
        if (ctx->point_idx < 0 || ctx->point_idx >= static_cast<int>(points.size())) {
            return vexc::Variant();
        }

        auto v = new sm::vec3(points[ctx->point_idx]->pos);
        return vexc::Variant(vexc::VarType::Float3, (void*)(v->xyz));
    });

    vexc::RegistBuildInFunc("@ptnum", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        auto ctx = static_cast<const EvalContext*>(ud);
        return vexc::Variant(ctx->point_idx);
    });

    // DESCRIBE

    vexc::RegistBuildInFunc("$SIZEX", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
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
    });

    vexc::RegistBuildInFunc("$SIZEY", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
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
    });

    vexc::RegistBuildInFunc("$SIZEZ", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
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
    });
}

}