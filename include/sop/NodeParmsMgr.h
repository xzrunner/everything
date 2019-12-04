#pragma once

#include <dag/Variable.h>

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

    void AddParm(const std::string& key, const dag::Variable& val);
    bool RemoveParm(const std::string& key);
    void ClearAllParms();
    size_t ExternalParmSize() const { return m_parms_map.size(); }

    bool SetValue(const std::string& key, const dag::Variable& val);
    bool SetArrayValue(const std::string& array_key, size_t idx,
        const dag::Variable& val, const std::string& sub_key = "");

    bool Update(const Evaluator& eval);

    dag::Variable Query(const std::string& key) const;

    bool IsExist(const std::string& key) const;

private:
    struct InKeyInfo
    {
        InKeyInfo(const std::string& name, rttr::property prop, int comp = -1)
            : name(name), prop(prop), comp(comp) {}

        std::string name;
        int comp = -1;

        rttr::property prop;
    };

    struct ExKeyInfo
    {
        ExKeyInfo(const std::string& name, const dag::Variable* var, int comp = -1)
            : name(name), var(var), comp(comp) {}

        std::string name;
        int comp = -1;

        const dag::Variable* var = nullptr;
    };

    InKeyInfo ParseInternalKey(const std::string& key) const;
    ExKeyInfo ParseExternalKey(const std::string& key) const;

    bool SetInternalVal(rttr::property prop, const dag::Variable& val, int comp_idx = -1);
    bool SetExternalVal(const dag::Variable* dst, const dag::Variable& val, int comp_idx = -1);

    bool SetInternalArrayVal(rttr::property prop, size_t idx,
        const dag::Variable& val, const std::string& sub_key);
    bool SetExternalArrayVal(const dag::Variable* dst, size_t idx,
        const dag::Variable& val, const std::string& sub_key);

private:
    Node& m_node;

    std::map<std::string, std::string> m_exprs_map;

    std::map<std::string, dag::Variable> m_parms_map;

}; // NodeParmsMgr

}