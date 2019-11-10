#pragma once

#include "sop/typedef.h"
#include "sop/PyCommonTypes.h"

#include <SM_Vector.h>

#include <string>

#include <boost/python.hpp>

namespace sop
{

class Evaluator;

namespace py
{

class Keyframe;
class StringKeyframe;

class Parm
{
public:
    Parm(const std::string& name, const sop::NodePtr& node)
        : m_name(name)
        , m_node(node)
    {
    }

    void SetKeyframe(const Keyframe& k) {}
    void SetKeyframe(const StringKeyframe& k) {}
    void Set(float val);
    void Set(const std::string& val);

private:
    std::string m_name;

    sop::NodePtr m_node = nullptr;

}; // Parm

class ParmTuple
{
public:
    ParmTuple(const std::string& name, const sop::NodePtr& node)
        : m_name(name)
        , m_node(node)
    {
    }

    void SetAutoscope(const boost::python::tuple& bool_values) {}
    void Set(const boost::python::tuple& val);
    Parm GetItem(size_t i);

private:
    std::string m_name;

    sop::NodePtr m_node = nullptr;

//    Parm m_parms[3];

}; // ParmTuple

class FolderParmTemplate;

class ParmTemplateGroup
{
public:
    void Append(const FolderParmTemplate& parm) {}

}; // ParmTemplateGroup

class Color
{
public:
    float r, g, b;

}; // Color

class Keyframe
{
public:
    void SetTime(float time) {}
    void SetExpression(const std::string& expression, ExprLanguage language) {}
    void InterpretAccelAsRatio(bool on) {}

}; // Keyframe

class StringKeyframe
{
public:
    void SetTime(float time) {}
    void SetExpression(const std::string& expression, ExprLanguage language) {}

}; // StringKeyframe

class NodeProxy
{
public:
    NodeProxy(const sop::NodePtr& node, Evaluator& eval);

    std::shared_ptr<NodeProxy> CreateNode(const std::string& type, const std::string& name,
        bool run_init_scripts = false, bool load_contents = false, bool exact_type_name = false);
    std::shared_ptr<NodeProxy> CreateNetworkBox(const std::string& name);
    std::shared_ptr<NodeProxy> FindNetworkBox(const std::string& name);

    std::shared_ptr<NodeProxy> GetParent();
    std::shared_ptr<NodeProxy> GetChild(const std::string& name);

    void Move(const sm::vec2& v) {}
    void SetSelectableInViewport(bool b) {}
    void ShowOrigin(bool b) {}
    void UseXray(bool b) {}
    void Hide(bool b) {}
    void SetSelected(bool b) {}
    void SetParmTemplateGroup(const ParmTemplateGroup& parm) {}
    ParmTuple GetParmTuple(const std::string& name) { return ParmTuple(name, m_node); }
    void SetExpressionLanguage(ExprLanguage lang) {}
    void SetUserData(const std::string& name, const std::string& value) {}
    void Bypass(bool on) {}
    void SetDisplayFlag(bool on) {}
    void SetHighlightFlag(bool on) {}
    void SetHardLocked(bool on) {}
    void SetSoftLocked(bool on) {}
    void SetSelectableTemplateFlag(bool on) {}
    void SetRenderFlag(bool on) {}
    void SetTemplateFlag(bool on) {}
    void SetUnloadFlag(bool on) {}
    void SetInput(int input_index, const std::shared_ptr<NodeProxy>& item_to_become_input, int output_index = 0);
    void SetPosition(const sm::vec2& pos) {}
    void SetSize(const sm::vec2& size) {}
    void SetMinimized(bool on) {}
    void SetAutoFit(bool on) {}
    void SetComment(const std::string& comment) {}
    void SetColor(const Color& col) {}
    Parm GetParm(const std::string& name) { return Parm(name, m_node); }
    boost::python::list IndirectInputs() { return boost::python::list(); }

private:
    sop::NodePtr m_node = nullptr;

    Evaluator& m_eval;

}; // NodeProxy

void BindNode();

}
}