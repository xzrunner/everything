#pragma once

#include "sop/Variable.h"
#include "sop/ParmType.h"

#include <rttr/type>

#include <map>
#include <string>

namespace sop
{

class Node;
class Evaluator;

class NodeParmsMgr
{
public:
    NodeParmsMgr(Node& node);

    void SetExpr(const std::string& key, const std::string& val);
    //void SetExpr(size_t id, const std::string& val, size_t comp_idx = 0);

    bool SetValue(const std::string& key, const Variable& val, bool float_cast = false);

    bool Update(const Evaluator& eval);

    Variable Query(const std::string& key) const;

private:
    struct KeyInfo
    {
        KeyInfo(const std::string& name, rttr::property prop, int comp = -1)
            : name(name), prop(prop), comp(comp) {}

        std::string name;
        int comp = -1;

        rttr::property prop;

        rttr::variant val;
        ParmType type;
    };

    KeyInfo ParseKey(const std::string& key) const;

private:
    Node& m_node;

//    std::map<size_t, std::string> m_exprs;
    std::map<std::string, std::string> m_exprs_map;

}; // NodeParmsMgr

}