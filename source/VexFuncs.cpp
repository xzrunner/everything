#include "everything/VexFuncs.h"
#include "everything/EvalContext.h"
#include "everything/GeometryImpl.h"

#include <vexc/EvalAST.h>
#include <vexc/StringPool.h>
#include <cpputil/StringHelper.h>

namespace evt
{

void SetupVexFuncs()
{
    vexc::RegistBuildInFunc("ch", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.empty()) {
            return vexc::Variant(false);
        }

        auto& p = params[0];
        if (p.type != vexc::VarType::String) {
            return vexc::Variant(false);
        }

        auto ctx = static_cast<const EvalContext*>(ud);

        std::string path(vexc::StringPool::VoidToString(p.p));
        std::vector<std::string> tokens;
        cpputil::StringHelper::Split(path, "/", tokens);
        auto curr_node = ctx->node;
        int curr_level = curr_node->GetLevel();
        for (size_t i = 0, n = tokens.size(); i < n; ++i)
        {
            auto& t = tokens[i];
            if (t == "..") {
                curr_node = curr_node->GetParent().get();
                --curr_level;
            } else {
                assert(curr_node && i == tokens.size() - 1);
                auto v = curr_node->GetProps().Query(t);
                switch (v.type)
                {
                case VariableType::Bool:
                    return vexc::Variant(v.b);
                case VariableType::Int:
                    return vexc::Variant(v.i);
                case VariableType::Float:
                    return vexc::Variant(v.f);
                case VariableType::String:
                    return vexc::Variant(vexc::VarType::String, v.c);
                default:
                    assert(0);
                    return vexc::Variant(false);
                }
            }
        }

        return vexc::Variant(false);
    });

    vexc::RegistBuildInFunc("@P", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.empty()) {
            return vexc::Variant(false);
        }

        auto& p = params[0];
        if (p.type != vexc::VarType::String) {
            return vexc::Variant(false);
        }

        auto ctx = static_cast<const EvalContext*>(ud);
        if (!ctx->node) {
            return vexc::Variant(false);
        }
        auto geo = ctx->node->GetGeometry();
        if (!geo) {
            return vexc::Variant(false);
        }
        const auto& points = geo->GetAttr().GetPoints();
        if (ctx->point_idx < 0 || ctx->point_idx >= points.size()) {
            return vexc::Variant(false);
        }

        auto& pos = points[ctx->point_idx]->pos;
        std::string mem(vexc::StringPool::VoidToString(p.p));
        if (mem == "x") {
            return vexc::Variant(pos.x);
        } else if (mem == "y") {
            return vexc::Variant(pos.y);
        } else if (mem == "z") {
            return vexc::Variant(pos.z);
        } else {
            return vexc::Variant(false);
        }
    });

    vexc::RegistBuildInFunc("fit", [](const std::vector<vexc::Variant>& params, const void* ud)->vexc::Variant
    {
        if (params.size() < 5) {
            return vexc::Variant(false);
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
}

}