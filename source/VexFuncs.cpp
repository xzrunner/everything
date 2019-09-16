#include "everything/VexFuncs.h"
#include "everything/EvalContext.h"
#include "everything/GeometryImpl.h"
#include "everything/Evaluator.h"
#include "everything/node/Geometry.h"

#include <vexc/EvalAST.h>
#include <vexc/StringPool.h>
#include <cpputil/StringHelper.h>

namespace
{

evt::Node* PathToNode(evt::Node* base_node, const std::string& path, const evt::Evaluator& eval)
{
    std::vector<std::string> tokens;
    cpputil::StringHelper::Split(path, "/", tokens);
    evt::Node* curr_node = base_node;
    int curr_level = curr_node->GetLevel();
    const int begin_level = curr_level;
    for (size_t i = 0, n = tokens.size(); i < n; ++i)
    {
        if (!curr_node && curr_level == begin_level - 1) {
            curr_node = eval.QueryNode(tokens[i]).get();
            if (curr_node) {
                continue;
            }
        }

        if (!curr_node) {
            return nullptr;
        }

        auto& t = tokens[i];
        if (t == "..") {
            curr_node = curr_node->GetParent().get();
            --curr_level;
            continue;
        }

        // query child
        assert(curr_node);
        if (curr_node->get_type() == rttr::type::get<evt::node::Geometry>())
        {
            auto child = static_cast<const evt::node::Geometry*>(curr_node)->QueryChild(t);
            if (child) {
                curr_node = child.get();
                continue;
            }
        }
    }
    return curr_node;
}

}

namespace evt
{

void SetupVexFuncs()
{
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
        evt::Node* curr_node = const_cast<evt::Node*>(ctx->node);
        int curr_level = curr_node->GetLevel();
        const int begin_level = curr_level;
        for (size_t i = 0, n = tokens.size(); i < n; ++i)
        {
            if (!curr_node && curr_level == begin_level - 1) {
                curr_node = ctx->eval->QueryNode(tokens[i]).get();
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
            case VariableType::Invalid:
                return vexc::Variant();
            case VariableType::Bool:
                return vexc::Variant(v.b);
            case VariableType::Int:
                return vexc::Variant(v.i);
            case VariableType::Float:
                return vexc::Variant(v.f);
            case VariableType::Float3:
            {
                auto buf = new float[3];
                memcpy(buf, v.p, sizeof(buf));
                return vexc::Variant(vexc::VarType::Float3, buf);
            }
            case VariableType::Double:
                return vexc::Variant(v.d);
            //case VariableType::String:
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

    // others

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
        evt::Node* base_node = const_cast<evt::Node*>(ctx->node);

        assert(params[0].type == vexc::VarType::String);
        std::string path(vexc::StringPool::VoidToString(params[0].p));
        auto surface_node = PathToNode(base_node, path, *ctx->eval);
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

        auto attrs = geo->GetAttr().QueryAttr(GeoAttrType::Primitive, attrib_name);
        if (!attrs) {
            return vexc::Variant();
        }

        if (attrib_index < 0 || attrib_index >= static_cast<int>(attrs->vars.size())) {
            return vexc::Variant();
        }
        auto& attr = attrs->vars[attrib_index];
        switch (attr.type)
        {
        case VariableType::Bool:
            return vexc::Variant(attr.b);
        case VariableType::Int:
            return vexc::Variant(attr.i);
        case VariableType::Float:
            return vexc::Variant(attr.f);
        case VariableType::Float3:
        {
            auto buf = new float[3];
            memcpy(buf, attr.p, sizeof(buf));
            assert(attrib_index >= 0 && attrib_index < 3);
            vexc::Variant ret(buf[attrib_index]);
            delete[] buf;
            return ret;
        }
        case VariableType::Double:
            return vexc::Variant(attr.d);
        default:
            assert(0);
            return vexc::Variant();
        }
    });
}

}