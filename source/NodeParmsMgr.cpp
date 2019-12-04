#include "sop/NodeParmsMgr.h"
#include "sop/Node.h"
#include "sop/Evaluator.h"
#include "sop/EvalContext.h"
#include "sop/RTTRInfo.h"
#include "sop/Widgets.h"
#include "sop/node/Add.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

namespace sop
{

NodeParmsMgr::NodeParmsMgr(Node& node)
    : m_node(node)
{
}

void NodeParmsMgr::SetExpr(const std::string& key, const std::string& val)
{
    m_exprs_map.insert({ key, val });
}

void NodeParmsMgr::AddParm(const std::string& key, const dag::Variable& val)
{
    m_parms_map.insert({ key, val });
}

bool NodeParmsMgr::RemoveParm(const std::string& key)
{
    auto itr = m_parms_map.find(key);
    if (itr == m_parms_map.end()) {
        return false;
    } else {
        m_parms_map.erase(itr);
        return true;
    }
}

void NodeParmsMgr::ClearAllParms()
{
    m_parms_map.clear();
}

bool NodeParmsMgr::SetValue(const std::string& key, const dag::Variable& val)
{
    auto in = ParseInternalKey(key);
    if (in.prop.is_valid()) {
        return SetInternalVal(in.prop, val, in.comp);
    }

    auto ex = ParseExternalKey(key);
    if (ex.var) {
        return SetExternalVal(ex.var, val, ex.comp);
    }

    return false;
}

bool NodeParmsMgr::SetArrayValue(const std::string& array_key, size_t idx,
                                 const dag::Variable& val, const std::string& sub_key)
{
    auto in = ParseInternalKey(array_key);
    if (in.prop.is_valid()) {
        return SetInternalArrayVal(in.prop, idx, val, sub_key);
    }

    auto ex = ParseExternalKey(array_key);
    if (ex.var) {
        return SetExternalArrayVal(ex.var, idx, val, sub_key);
    }

    return false;
}

bool NodeParmsMgr::Update(const Evaluator& eval)
{
    if (m_exprs_map.empty()) {
        return false;
    }

    bool dirty = false;
    for (auto& itr : m_exprs_map)
    {
        auto& name = itr.first;
        auto& expr = itr.second;

        auto val = eval.CalcExpr(expr, EvalContext(eval, m_node));
        if (val.type == dag::VarType::String) {
            auto str = static_cast<const char*>(val.p);
            val = eval.CalcExpr(str, EvalContext(eval, m_node));
        }

        if (SetValue(name, val)) {
            dirty = true;
        }
    }

    return dirty;
}

dag::Variable NodeParmsMgr::Query(const std::string& key) const
{
    dag::Variable ret;

    auto in = ParseInternalKey(key);
    if (in.prop.is_valid())
    {
        auto type = in.prop.get_type();
        if (type == rttr::type::get<float>())
        {
            auto f = in.prop.get_value(m_node).get_value<float>();
            return dag::Variable(f);
        }
        else if (type == rttr::type::get<sm::vec3>())
        {
            auto v3 = in.prop.get_value(m_node).get_value<sm::vec3>();
            switch (key.back())
            {
            case 'x':
                return dag::Variable(v3.x);
            case 'y':
                return dag::Variable(v3.y);
            case 'z':
                return dag::Variable(v3.z);
            default:
                assert(0);
            }
        }
        else
        {
            assert(0);
        }

        return dag::Variable();
    }

    auto ex = ParseExternalKey(key);
    if (ex.var) {
        return *ex.var;
    }

    return dag::Variable();
}

bool NodeParmsMgr::IsExist(const std::string& key) const
{
    auto in = ParseInternalKey(key);
    if (in.prop.is_valid()) {
        return true;
    }

    auto ex = ParseExternalKey(key);
    if (ex.var) {
        return true;
    }

    return false;
}

NodeParmsMgr::InKeyInfo
NodeParmsMgr::ParseInternalKey(const std::string& key) const
{
    auto node_type = m_node.get_type();
    auto prop = node_type.get_property(key);
    if (prop.is_valid()) {
        return InKeyInfo(key, prop);
    }

    if (!key.empty())
    {
        int comp = -1;

        auto perfix = key.back();
        switch (perfix)
        {
        case 'x':
            comp = 0;
            break;
        case 'y':
            comp = 1;
            break;
        case 'z':
            comp = 2;
            break;
        }

        if (comp >= 0)
        {
            auto name = key.substr(0, key.size() - 1);
            auto prop = node_type.get_property(name);
            if (prop.is_valid()) {
                return InKeyInfo(name, prop, comp);
            } else {
                return InKeyInfo("", prop, -1);
            }
        }
    }

    return InKeyInfo("", prop, -1);
}

NodeParmsMgr::ExKeyInfo
NodeParmsMgr::ParseExternalKey(const std::string& key) const
{
    auto itr = m_parms_map.find(key);
    if (itr != m_parms_map.end()) {
        return ExKeyInfo(key, &itr->second);
    }

    if (!key.empty())
    {
        int comp = -1;

        auto perfix = key.back();
        switch (perfix)
        {
        case 'x':
            comp = 0;
            break;
        case 'y':
            comp = 1;
            break;
        case 'z':
            comp = 2;
            break;
        }

        if (comp >= 0)
        {
            auto name = key.substr(0, key.size() - 1);
            auto itr = m_parms_map.find(name);
            if (itr != m_parms_map.end()) {
                return ExKeyInfo(key, &itr->second, comp);
            } else {
                return ExKeyInfo("", nullptr);
            }
        }
    }

    return ExKeyInfo("", nullptr);
}

bool NodeParmsMgr::SetInternalVal(rttr::property prop, const dag::Variable& val, int comp_idx)
{
    assert(prop.is_valid());

    auto var = prop.get_value(m_node);
    assert(var.is_valid());

    auto src_type = val.type;
    auto dst_type = prop.get_type();

    if (prop.is_enumeration())
    {
        switch (val.type)
        {
        case dag::VarType::Float:
        {
            auto idx = static_cast<int>(val.f);
            assert(fabs(val.f - idx) < FLT_EPSILON);
            if (idx == var.get_value<int>()) {
                return false;
            }
            auto vars = dst_type.get_enumeration().get_values();
            assert(idx >= 0 && idx < static_cast<int>(vars.size()));
            bool find = false;
            for (auto& var : vars)
            {
                if (var.to_int() != idx) {
                    continue;
                }
                auto succ = prop.set_value(m_node, var);
                assert(succ);
                find = true;
                break;
            }
            assert(find);
            return true;
        }
            break;
        case dag::VarType::String:
        {
            auto dst = var.get_value<int>();
            auto src = prop.get_enumeration().name_to_value(static_cast<const char*>(val.p));
            if (src.get_value<int>() == dst) {
                return false;
            } else {
                auto succ = prop.set_value(m_node, src);
                assert(succ);
                return true;
            }
        }
            break;
        default:
            assert(0);
        }
    }
    else if (dst_type == rttr::type::get<bool>())
    {
        switch (src_type)
        {
        case dag::VarType::Float:
        {
            auto dst = var.get_value<bool>();
            assert(val.f == 0.0f || val.f == 1.0f);
            auto src = val.f == 0.0f ? false : true;
            if (src == dst) {
                return false;
            } else {
                auto succ = prop.set_value(m_node, src);
                assert(succ);
                return true;
            }
        }
            break;
        default:
            assert(0);
        }
    }
    else if (dst_type == rttr::type::get<int>())
    {
        switch (src_type)
        {
        case dag::VarType::Float:
        {
            auto dst = var.get_value<int>();
            auto src = static_cast<int>(val.f);
            assert(fabs(val.f - src) < FLT_EPSILON);
            if (src == dst) {
                return false;
            } else {
                auto succ = prop.set_value(m_node, src);
                assert(succ);
                return true;
            }
        }
            break;
        default:
            assert(0);
        }
    }
    else if (dst_type == rttr::type::get<unsigned int>())
    {
        switch (src_type)
        {
        case dag::VarType::Float:
        {
            auto dst = var.get_value<unsigned int>();
            auto src = static_cast<unsigned int>(val.f);
            assert(fabs(val.f - src) < FLT_EPSILON);
            if (src == dst) {
                return false;
            } else {
                auto succ = prop.set_value(m_node, src);
                assert(succ);
                return true;
            }
        }
            break;
        default:
            assert(0);
        }
    }
    else if (dst_type == rttr::type::get<float>())
    {
        switch (src_type)
        {
        case dag::VarType::Float:
            if (var.get_value<float>() == val.f) {
                return false;
            } else {
                auto succ = prop.set_value(m_node, val.f);
                assert(succ);
                return true;
            }
            break;
        default:
            assert(0);
        }
    }
    else if (dst_type == rttr::type::get<sm::vec3>())
    {
        switch (src_type)
        {
        case dag::VarType::Int:
        {
            assert(comp_idx >= 0 && comp_idx < 3);
            auto dst = var.get_value<sm::vec3>();
            auto src = static_cast<float>(val.i);
            if (dst.xyz[comp_idx] == src) {
                return false;
            } else {
                dst.xyz[comp_idx] = src;
                auto succ = prop.set_value(m_node, dst);
                assert(succ);
                return true;
            }
        }
            break;
        case dag::VarType::Float:
        {
            assert(comp_idx >= 0 && comp_idx < 3);
            auto dst = var.get_value<sm::vec3>();
            auto src = val.f;
            if (dst.xyz[comp_idx] == src) {
                return false;
            } else {
                dst.xyz[comp_idx] = src;
                auto succ = prop.set_value(m_node, dst);
                assert(succ);
                return true;
            }
        }
            break;
        case dag::VarType::Float3:
        {
            auto dst = var.get_value<sm::vec3>();
            auto src = static_cast<const sm::vec3*>(val.p);
            if (*src == dst) {
                return false;
            } else {
                auto succ = prop.set_value(m_node, *src);
                assert(succ);
                return true;
            }
        }
            break;
        default:
            assert(0);
        }
    }
    else if (dst_type == rttr::type::get<std::string>())
    {
        auto dst = var.get_value<std::string>();
        std::string src;
        switch (val.type)
        {
        case dag::VarType::Float:
            src = std::to_string(val.f);
            break;
        case dag::VarType::String:
            src = static_cast<const char*>(val.p);
            break;
        default:
            assert(0);
        }

        if (dst == src) {
            return false;
        } else {
            auto succ = prop.set_value(m_node, src);
            assert(succ);
            return true;
        }
    }
    else if (dst_type == rttr::type::get<std::vector<std::string>>())
    {
        auto dst = var.get_value<std::vector<std::string>>();
        switch (val.type)
        {
        case dag::VarType::String:
        {
            std::vector<std::string> src;
            std::string s = static_cast<const char*>(val.p);
            boost::split(src, s, boost::is_any_of(" "));
            if (dst == src) {
                false;
            } else {
                auto succ = prop.set_value(m_node, src);
                assert(succ);
                return true;
            }
        }
            break;
        }
    }
    else
    {
        assert(0);
    }

    assert(0);
    return false;
}

bool NodeParmsMgr::SetExternalVal(const dag::Variable* dst, const dag::Variable& val, int comp_idx)
{
    assert(dst);
    if (dst->type == val.type)
    {
        assert(comp_idx < 0);
        const_cast<dag::Variable&>(*dst) = val;
        return true;
    }
    else if (dst->type == dag::VarType::Float3 && val.type == dag::VarType::Float && comp_idx >= 0)
    {
        auto v3 = static_cast<const sm::vec3*>(val.p);
        assert(comp_idx <= 2);
        const_cast<sm::vec3*>(v3)->xyz[comp_idx] = val.f;
        return true;
    }

    return false;
}

bool NodeParmsMgr::SetInternalArrayVal(rttr::property prop, size_t idx,
                                       const dag::Variable& val, const std::string& sub_key)
{
    assert(prop.is_valid());

    if (m_node.get_type() != rttr::type::get<node::Add>()) {
        assert(idx > 0);
        --idx;
    }

    assert(prop.get_type().is_sequential_container());
    auto var = prop.get_value(m_node);
    assert(var.is_valid());
    auto view = var.create_sequential_view();
    assert(view.is_valid());
    if (view.get_size() < idx + 1) {
        auto succ = view.set_size(idx + 1);
        assert(succ);
    }

    auto src_type = val.type;
    switch (src_type)
    {
    case dag::VarType::Float:
    {
        auto dst_type = view.get_value(idx).get_type();
        auto dst_wrapped_type = view.get_value(idx).extract_wrapped_value().get_type();
        if (dst_type == rttr::type::get<bool>())
        {
            auto dst = view.get_value(idx).get_value<bool>();
            assert(val.f == 0.0f || val.f == 1.0f);
            auto src = val.f == 0.0f ? false : true;
            if (src == dst) {
                return false;
            } else {
                auto succ = view.set_value(idx, src);
                assert(succ);
                return true;
            }
        }
        else if (dst_wrapped_type == rttr::type::get<int>())
        {
            auto dst = view.get_value(idx).get_value<int>();
            auto src = static_cast<int>(val.f);
            assert(fabs(src - val.f) < FLT_EPSILON);
            if (src == dst) {
                return false;
            } else {
                auto succ = view.set_value(idx, src);
                assert(succ);
                return true;
            }
        }
        else if (dst_wrapped_type == rttr::type::get<RampFloat>())
        {
            auto dst = view.get_value(idx).get_wrapped_value<RampFloat>();
            auto src = val.f;
            if (sub_key == RampFloat::COMP_NAMES[RampFloat::COMP_VALUE])
            {
                if (src == dst.value) {
                    return false;
                } else {
                    dst.value = src;
                    auto succ = view.set_value(idx, dst);
                    assert(succ);
                    return true;
                }
            }
            else if (sub_key == RampFloat::COMP_NAMES[RampFloat::COMP_POS])
            {
                if (src == dst.pos) {
                    return false;
                } else {
                    dst.pos = src;
                    auto succ = view.set_value(idx, dst);
                    assert(succ);
                    return true;
                }
            }
            else
            {
                assert(0);
            }
        }
        else if (dst_wrapped_type == rttr::type::get<RampColor>())
        {
            auto dst = view.get_value(idx).get_wrapped_value<RampColor>();
            auto src = val.f;
            if (sub_key == RampColor::COMP_NAMES[RampColor::COMP_POS])
            {
                if (src == dst.pos) {
                    return false;
                } else {
                    dst.pos = src;
                    auto succ = view.set_value(idx, dst);
                    assert(succ);
                    return true;
                }
            }
            else
            {
                assert(0);
            }
        }
        else
        {
            assert(0);
        }
    }
        break;
    case dag::VarType::Float3:
    {
        auto dst_wrapped_type = view.get_value(idx).extract_wrapped_value().get_type();
        if (dst_wrapped_type == rttr::type::get<sm::vec3>())
        {
            auto dst = view.get_value(idx).get_value<sm::vec3>();
            auto src = static_cast<const sm::vec3*>(val.p);
            if (*src == dst) {
                return false;
            } else {
                auto succ = view.set_value(idx, *src);
                assert(succ);
                return true;
            }
        }
        else if (dst_wrapped_type == rttr::type::get<sm::vec4>())
        {
            auto dst = view.get_value(idx).get_value<sm::vec4>();
            auto src = static_cast<const sm::vec3*>(val.p);
            if (*src == sm::vec3(dst.xyzw)) {
                return false;
            } else {
                dst.x = src->x;
                dst.y = src->y;
                dst.z = src->z;
                auto succ = view.set_value(idx, dst);
                assert(succ);
                return true;
            }
        }
        else if (dst_wrapped_type == rttr::type::get<RampColor>() && sub_key == RampColor::COMP_NAMES[RampColor::COMP_COLOR])
        {
            auto dst = view.get_value(idx).get_wrapped_value<RampColor>();
            auto src = static_cast<const sm::vec3*>(val.p);
            if (*src == dst.color) {
                return false;
            } else {
                dst.color = *src;
                auto succ = view.set_value(idx, dst);
                assert(succ);
                return true;
            }
        }
        else
        {
            assert(0);
        }
    }
        break;
    case dag::VarType::String:
    {
        auto dst_wrapped_type = view.get_value(idx).extract_wrapped_value().get_type();
        if (dst_wrapped_type.is_enumeration())
        {
            auto dst = view.get_value(idx).get_value<int>();
            auto src = dst_wrapped_type.get_enumeration().name_to_value(static_cast<const char*>(val.p));
            if (src.get_value<int>() == dst) {
                return false;
            } else {
                auto succ = view.set_value(idx, src);
                assert(succ);
                return true;
            }
        }
        else if (dst_wrapped_type == rttr::type::get<std::string>())
        {
            std::string dst = view.get_value(idx).get_value<const char*>();
            std::string src = static_cast<const char*>(val.p);
            if (dst == src) {
                return false;
            } else {
                auto succ = view.set_value(idx, src);
                assert(succ);
                return true;
            }
        }
        else if (dst_wrapped_type == rttr::type::get<RampFloat>())
        {
            auto dst = view.get_value(idx).get_wrapped_value<RampFloat>();
            std::string src = static_cast<const char*>(val.p);
            if (sub_key == RampFloat::COMP_NAMES[RampFloat::COMP_INTERP])
            {
                auto src_interp = rttr::type::get<InterpType>().get_enumeration().name_to_value(src).get_value<InterpType>();
                if (src_interp == dst.interp) {
                    return false;
                } else {
                    dst.interp = src_interp;
                    auto succ = view.set_value(idx, dst);
                    assert(succ);
                    return true;
                }
            }
            else
            {
                assert(0);
            }
        }
        else if (dst_wrapped_type == rttr::type::get<RampColor>())
        {
            auto dst = view.get_value(idx).get_wrapped_value<RampColor>();
            std::string src = static_cast<const char*>(val.p);
            if (sub_key == RampColor::COMP_NAMES[RampColor::COMP_INTERP])
            {
                auto src_interp = rttr::type::get<InterpType>().get_enumeration().name_to_value(src).get_value<InterpType>();
                if (src_interp == dst.interp) {
                    return false;
                } else {
                    dst.interp = src_interp;
                    auto succ = view.set_value(idx, dst);
                    assert(succ);
                    return true;
                }
            }
            else
            {
                assert(0);
            }
        }
        else
        {
            assert(0);
        }
    }
    default:
        assert(0);
    }

    return false;
}

bool NodeParmsMgr::SetExternalArrayVal(const dag::Variable* dst, size_t idx,
                                       const dag::Variable& val, const std::string& sub_key)
{
    assert(0);
    return false;
}

}