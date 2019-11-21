#include "sop/NodeParmsMgr.h"
#include "sop/Node.h"
#include "sop/Evaluator.h"
#include "sop/EvalContext.h"
#include "sop/ParmType.h"
#include "sop/RTTRInfo.h"

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

//void NodeParmsMgr::SetExpr(size_t id, const std::string& val, size_t comp_idx)
//{
//    m_exprs_map.insert({ id * 3 + comp_idx, val });
//}

bool NodeParmsMgr::SetValue(const std::string& key, const Variable& val, bool float_cast)
{
    auto key_info = ParseKey(key);
    // todo
    if (!key_info.prop.is_valid()) {
        return false;
    }
    assert(key_info.prop.is_valid());

    auto src_type = val.type;
    auto dst_type = key_info.type;
    switch (dst_type)
    {
    case ParmType::Integer:
    {
        switch (src_type)
        {
        case VarType::Float:
        {
            auto dst = key_info.val.get_value<int>();
            auto src = static_cast<int>(val.f);
            assert(fabs(val.f - src) < FLT_EPSILON);
            if (src == dst) {
                return false;
            } else {
                auto succ = key_info.prop.set_value(m_node, src);
                assert(succ);
                return true;
            }
        }
            break;
        default:
            assert(0);
        }
    }
        break;
    case ParmType::Float:
    {
        switch (src_type)
        {
        case VarType::Float:
            if (key_info.val.get_value<float>() == val.f) {
                return false;
            } else {
                auto succ = key_info.prop.set_value(m_node, val.f);
                assert(succ);
                return true;
            }
            break;
        default:
            assert(0);
        }
    }
        break;
    case ParmType::Vector3:
    {
        switch (src_type)
        {
        case VarType::Int:
        {
            assert(key_info.comp >= 0 && key_info.comp < 3);
            auto dst = key_info.val.get_value<sm::vec3>();
            auto src = static_cast<float>(val.i);
            if (dst.xyz[key_info.comp] == src) {
                return false;
            } else {
                dst.xyz[key_info.comp] = src;
                auto succ = key_info.prop.set_value(m_node, dst);
                assert(succ);
                return true;
            }
        }
            break;
        case VarType::Float:
        {
            assert(key_info.comp >= 0 && key_info.comp < 3);
            auto dst = key_info.val.get_value<sm::vec3>();
            auto src = val.f;
            if (dst.xyz[key_info.comp] == src) {
                return false;
            } else {
                dst.xyz[key_info.comp] = src;
                auto succ = key_info.prop.set_value(m_node, dst);
                assert(succ);
                return true;
            }
        }
            break;
        case VarType::Float3:
        {
            auto dst = key_info.val.get_value<sm::vec3>();
            auto src = static_cast<const sm::vec3*>(val.p);
            if (*src == dst) {
                return false;
            } else {
                auto succ = key_info.prop.set_value(m_node, *src);
                assert(succ);
                return true;
            }
        }
            break;
        default:
            assert(0);
        }
    }
        break;
    default:
        assert(0);
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
        if (val.type == VarType::String) {
            auto str = static_cast<const char*>(val.p);
            val = eval.CalcExpr(str, EvalContext(eval, m_node));
        }

        if (SetValue(name, val)) {
            dirty = true;
        }
    }

    return dirty;
}

Variable NodeParmsMgr::Query(const std::string& key) const
{
    Variable ret;

    auto key_info = ParseKey(key);
    if (!key_info.prop.is_valid()) {
        return ret;
    }
    assert(key_info.prop.is_valid());

    switch (key_info.type)
    {
    case ParmType::Float:
    {
        auto f = key_info.val.get_value<float>();
        return Variable(f);
    }
        break;
    case ParmType::Vector3:
    {
        auto v3 = key_info.val.get_value<sm::vec3>();
        switch (key.back())
        {
        case 'x':
            return Variable(v3.x);
        case 'y':
            return Variable(v3.y);
        case 'z':
            return Variable(v3.z);
        default:
            assert(0);
        }
    }
        break;
    default:
        assert(0);
    }

    return Variable();
}

NodeParmsMgr::KeyInfo NodeParmsMgr::ParseKey(const std::string& key) const
{
    auto node_type = m_node.get_type();
    auto prop = node_type.get_property(key);
    if (prop.is_valid())
    {
        KeyInfo ret(key, prop);
        ret.val = prop.get_value(m_node);
        assert(ret.val.is_valid());
        ret.type = prop.get_metadata(PropMeta::ParmType).get_value<ParmType>();
        return ret;
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
            if (!prop.is_valid()) {
                return KeyInfo("", prop, -1);
            }
            assert(prop.is_valid());
            KeyInfo ret(name, prop, comp);
            ret.val = prop.get_value(m_node);
            assert(ret.val.is_valid());
            ret.type = prop.get_metadata(PropMeta::ParmType).get_value<ParmType>();
            return ret;
        }
    }

    return KeyInfo("", prop, -1);
}

}