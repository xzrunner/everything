#pragma once

#include "sop/Variable.h"

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

    bool SetValue(const std::string& key, const Variable& val);
    bool SetArrayValue(const std::string& array_key, size_t idx,
        const Variable& val, const std::string& sub_key = "");

    bool Update(const Evaluator& eval);

    Variable Query(const std::string& key) const;

    bool IsExist(const std::string& key) const;

private:
    struct KeyInfo
    {
        KeyInfo(const std::string& name, rttr::property prop, int comp = -1)
            : name(name), prop(prop), comp(comp) {}

        std::string name;
        int comp = -1;

        rttr::property prop;
//        rttr::variant  val;
    };

    KeyInfo ParseKey(const std::string& key) const;

    bool SetValue(rttr::property prop, const Variable& val, int comp_idx = -1);

private:
    Node& m_node;

//    std::map<size_t, std::string> m_exprs;
    std::map<std::string, std::string> m_exprs_map;

}; // NodeParmsMgr

}