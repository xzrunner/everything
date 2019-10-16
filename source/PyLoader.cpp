#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyLoader.h"
#include "sop/typedef.h"
#include "sop/Node.h"
#include "sop/Evaluator.h"

#include "sop/node/Geometry.h"
// attribute
#include "sop/node/AttributeCreate.h"
#include "sop/node/AttributeWrangle.h"
#include "sop/node/Sort.h"
#include "sop/node/Measure.h"
// group
#include "sop/node/GroupCreate.h"
#include "sop/node/GroupExpression.h"
#include "sop/node/GroupPromote.h"
// manipulate
#include "sop/node/Delete.h"
#include "sop/node/Transform.h"
// material
#include "sop/node/Color.h"
// nurbs
#include "sop/node/Carve.h"
// polygon
#include "sop/node/Add.h"
#include "sop/node/Boolean.h"
#include "sop/node/Fuse.h"
#include "sop/node/Knife.h"
#include "sop/node/Normal.h"
#include "sop/node/PolyExtrude.h"
#include "sop/node/PolyFrame.h"
// primitive
#include "sop/node/Box.h"
#include "sop/node/Curve.h"
#include "sop/node/Line.h"
#include "sop/node/Primitive.h"
#include "sop/node/Sphere.h"
// utility
#include "sop/node/Blast.h"
#include "sop/node/CopyToPoints.h"
#include "sop/node/Merge.h"
#include "sop/node/ForeachPrimEnd.h"
#include "sop/node/Switch.h"

#include <SM_Vector.h>

#include <fstream>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

namespace
{

sop::Evaluator* EVAL = nullptr;

//////////////////////////////////////////////////////////////////////////
// enums
//////////////////////////////////////////////////////////////////////////

enum class ExprLanguage
{
    Python,
    Hscript,
};

enum class ScriptLanguage
{
    Python,
    Hscript,
};

// parms

enum class FolderType
{
    Collapsible,
    Simple,
    Tabs,
    RadioButtons,
    MultiparmBlock,
    ScrollingMultiparmBlock,
    TabbedMultiparmBlock,
    ImportBlock,
};

enum class MenuType
{
    Normal,
    Mini,
    StringReplace,
    StringToggle,
};

enum class StringParmType
{
    Regular,
    FileReference,
    NodeReference,
    NodeReferenceList,
};

enum class ParmLook
{
    Regular,
    Logarithmic,
    Angle,
    Vector,
    ColorSquare,
    HueCircle,
    CRGBAPlaneChooser,
};

enum class ParmNamingScheme
{
    Base1,
    XYZW,
    XYWH,
    UVW,
    RGBA,
    MinMax,
    MaxMin,
    StartEnd,
    BeginEnd,
};

enum class ParmCondType
{
    DisableWhen,
    HideWhen,
};

//////////////////////////////////////////////////////////////////////////
// types
//////////////////////////////////////////////////////////////////////////

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
    void Set(float val) 
    {
        assert(m_node);
        auto& props = const_cast<sop::NodePropsMgr&>(m_node->GetProps());
        props.SetValue(m_name, sop::Variable(val));
    }
    void Set(const std::string& val) 
    {
        assert(m_node);
        auto& props = const_cast<sop::NodePropsMgr&>(m_node->GetProps());
        props.SetValue(m_name, sop::Variable(val));
    }

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

    void SetAutoscope(const tuple& bool_values) {}
    void Set(const tuple& val) 
    {
        assert(m_node);
        auto& props = const_cast<sop::NodePropsMgr&>(m_node->GetProps());
        auto var = props.Query(m_name + "x");
        assert(var.type != sop::VarType::Invalid);
        switch (var.type)
        {
        case sop::VarType::Float:
            props.SetValue(m_name + "x", sop::Variable(boost::python::extract<float>(val[0])));
            props.SetValue(m_name + "y", sop::Variable(boost::python::extract<float>(val[1])));
            props.SetValue(m_name + "z", sop::Variable(boost::python::extract<float>(val[2])));
            break;
        default:
            assert(0);
        }
    }
    Parm GetItem(size_t i) 
    {
        switch (i)
        {
        case 0:
            return Parm(m_name + "x", m_node);
        case 1:
            return Parm(m_name + "y", m_node);
        case 2:
            return Parm(m_name + "z", m_node);
        default:
            assert(0);
            return Parm("", nullptr);
        }
    }

private:
    std::string m_name;

    sop::NodePtr m_node = nullptr;

//    Parm m_parms[3];

}; // ParmTuple

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

//////////////////////////////////////////////////////////////////////////
// node
//////////////////////////////////////////////////////////////////////////

class ParmTemplateGroup;

class NodeProxy
{
public:
    NodeProxy(const sop::NodePtr& node)
        : m_node(node)
    {
    }

    std::shared_ptr<NodeProxy> CreateNode(const std::string& type, const std::string& name,
        bool run_init_scripts = false, bool load_contents = false, bool exact_type_name = false)
    {
        assert(m_node);
        auto parent = std::dynamic_pointer_cast<sop::node::Geometry>(m_node);
        assert(parent);

        sop::NodePtr child = nullptr;
        // others
        if (type == "geo" || type == "geometry") {
            child = std::make_shared<sop::node::Geometry>();
        }
        // attribute
        else if (type == "attribcreate::2.0") {
            child = std::make_shared<sop::node::AttributeCreate>();
        } else if (type == "attribwrangle") {
            child = std::make_shared<sop::node::AttributeWrangle>();
        } else if (type == "measure") {
            child = std::make_shared<sop::node::Measure>();
        } else if (type == "sort") {
            child = std::make_shared<sop::node::Sort>();
        }
        // group
        else if (type == "groupcreate") {
            child = std::make_shared<sop::node::GroupCreate>();
        } else if (type == "groupexpression") {
            child = std::make_shared<sop::node::GroupExpression>();
        } else if (type == "grouppromote") {
            child = std::make_shared<sop::node::GroupPromote>();
        }
        // manipulate
        else if (type == "delete") {
            child = std::make_shared<sop::node::Delete>();
        } else if (type == "xform") {
            child = std::make_shared<sop::node::Transform>();
        }
        // nurbs
        else if (type == "carve") {
            child = std::make_shared<sop::node::Carve>();
        }
        // polygon
        else if (type == "add") {
            child = std::make_shared<sop::node::Add>();
        } else if (type == "fuse") {
            child = std::make_shared<sop::node::Fuse>();
        } else if (type == "knife") {
            child = std::make_shared<sop::node::Knife>();
        } else if (type == "normal") {
            child = std::make_shared<sop::node::Normal>();
        } else if (type == "polyextrude::2.0") {
            child = std::make_shared<sop::node::PolyExtrude>();
        } else if (type == "polyframe") {
            child = std::make_shared<sop::node::PolyFrame>();
        }
        // primitive
        else if (type == "box") {
            child = std::make_shared<sop::node::Box>();
        } else if (type == "line") {
            child = std::make_shared<sop::node::Line>();
        } else if (type == "primitive") {
            child = std::make_shared<sop::node::Primitive>();
        }
        // utility
        else if (type == "blast") {
            child = std::make_shared<sop::node::Blast>();
        } else if (type == "copytopoints") {
            child = std::make_shared<sop::node::CopyToPoints>();
        } else if (type == "block_begin" || type == "block_end") {
            child = std::make_shared<sop::node::ForeachPrimEnd>();
        } else if (type == "merge") {
            child = std::make_shared<sop::node::Merge>();
        }  else if (type == "switch") {
            child = std::make_shared<sop::node::Switch>();
        }
        // unknown
        else {
            printf("unknown node type %s\n", type.c_str());
            assert(0);
        }
        assert(child);

        EVAL->AddNode(child);

        child->SetName(name);
        sop::node::Geometry::AddChild(parent, child);

        return std::make_shared<NodeProxy>(child);
    }
    std::shared_ptr<NodeProxy> CreateNetworkBox(const std::string& name)
    {
        return std::make_shared<NodeProxy>(nullptr);
    }
    std::shared_ptr<NodeProxy> FindNetworkBox(const std::string& name)
    {
        assert(0);
        return nullptr;
    }

    std::shared_ptr<NodeProxy> GetParent()
    {
        if (m_node) {
            auto p = m_node->GetParent();
            if (p) {
                return std::make_shared<NodeProxy>(p);
            }
        }
        return nullptr;
    }
    std::shared_ptr<NodeProxy> GetChild(const std::string& name)
    {
        assert(m_node);
        auto parent = std::dynamic_pointer_cast<sop::node::Geometry>(m_node);
        assert(parent);

        auto child = parent->QueryChild(name);
        if (child) {
            return std::make_shared<NodeProxy>(child);
        } else {
            return nullptr;
        }
    }

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
    void SetInput(int input_index, const std::shared_ptr<NodeProxy>& item_to_become_input, int output_index = 0) 
    {
        if (m_node && item_to_become_input->m_node) {
            sop::make_connecting({ item_to_become_input->m_node, output_index }, { m_node, input_index });
        }
    }
    void SetPosition(const sm::vec2& pos) {}
    void SetSize(const sm::vec2& size) {}
    void SetMinimized(bool on) {}
    void SetAutoFit(bool on) {}
    void SetComment(const std::string& comment) {}
    void SetColor(const Color& col) {}
    Parm GetParm(const std::string& name) { return Parm(name, m_node); }
    list IndirectInputs() { return list(); }

private:
    sop::NodePtr m_node = nullptr;

}; // NodeProxy

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_create_node_overloads, NodeProxy::CreateNode, 2, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_set_input_overloads, NodeProxy::SetInput, 2, 3)

//////////////////////////////////////////////////////////////////////////
// parms
//////////////////////////////////////////////////////////////////////////

// ParmTemplateGroup

class FolderParmTemplate;

class ParmTemplateGroup
{
public:
    void Append(const FolderParmTemplate& parm) {}

}; // ParmTemplateGroup

ParmTemplateGroup hou_create_parm_template_group()
{
    return ParmTemplateGroup();
}

// FolderParmTemplate

class MenuParmTemplate;
class FloatParmTemplate;
class ToggleParmTemplate;
class StringParmTemplate;
class IntParmTemplate;

class FolderParmTemplate
{
public:
    void AddParmTemplate(const MenuParmTemplate& menu) {}
    void AddParmTemplate(const FloatParmTemplate& menu) {}
    void AddParmTemplate(const ToggleParmTemplate& menu) {}
    void AddParmTemplate(const StringParmTemplate& menu) {}
    void AddParmTemplate(const IntParmTemplate& menu) {}
    void AddParmTemplate(const FolderParmTemplate& menu) {}

}; // FolderParmTemplate

FolderParmTemplate hou_create_folder_parm_template(const std::string& name, const std::string& type,
    FolderType folder_type = FolderType::Tabs, int default_value = 0, bool ends_tab_group = false)
{
    return FolderParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_folder_parm_template_overloads, hou_create_folder_parm_template, 2, 5)

// MenuParmTemplate

class MenuParmTemplate
{
public:
    void SetJoinWithNext(bool b) {}
    void SetTags(const dict& tags) {}
    void Hide(bool b) {}
    void SetHelp(const std::string& help) {}

}; // MenuParmTemplate

MenuParmTemplate hou_create_menu_parm_template(const std::string& name, const std::string& type,
    const list& menu_items = {},
    const list& menu_labels = {},
    int default_value = 0, const list& icon_names = {}, const std::string& item_generator_script = "",
    ScriptLanguage item_generator_script_language = ScriptLanguage::Python, MenuType menu_type = MenuType::Normal)
{
    return MenuParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_menu_parm_template_overloads, hou_create_menu_parm_template, 2, 9)

// FloatParmTemplate

class FloatParmTemplate
{
public:
    void SetTags(const dict& tags) {}
    void Hide(bool b) {}
    void SetConditional(ParmCondType type, const std::string& val) {}

}; // FloatParmTemplate

FloatParmTemplate hou_create_float_parm_template(const std::string& name, const std::string& type, int val,
    const list& default_value = {}, float min = 0, float max = 360, bool min_is_strict = false, bool max_is_strict = false,
    ParmLook look = ParmLook::Regular, ParmNamingScheme naming_scheme = ParmNamingScheme::XYZW)
{
    return FloatParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_float_parm_template_overloads, hou_create_float_parm_template, 3, 10)

// ToggleParmTemplate

class ToggleParmTemplate
{
public:
    void SetJoinWithNext(bool b) {}
    void SetTags(const dict& tags) {}
    void SetConditional(ParmCondType type, const std::string& val) {}
    void Hide(bool b) {}

}; // ToggleParmTemplate

ToggleParmTemplate hou_create_toggle_parm_template(const std::string& name, const std::string& type, bool default_value = false)
{
    return ToggleParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_toggle_parm_template_overloads, hou_create_toggle_parm_template, 2, 3)

// StringParmTemplate

class StringParmTemplate
{
public:
    void SetConditional(ParmCondType type, const std::string& val) {}
    void SetTags(const dict& tags) {}
    void Hide(bool b) {}
    void SetHelp(const std::string& help) {}

}; // StringParmTemplate

StringParmTemplate hou_create_string_parm_template(const std::string& name, const std::string& type, int value, const list& default_value = {},
    ParmNamingScheme naming_scheme = ParmNamingScheme::Base1, StringParmType string_type = StringParmType::NodeReference,
    const list& menu_items = {}, const list& menu_labels = {}, const list& icon_names = {}, const std::string& item_generator_script = "",
    ScriptLanguage item_generator_script_language = ScriptLanguage::Python, MenuType menu_type = MenuType::Normal)
{
    return StringParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_string_parm_template_overloads, hou_create_string_parm_template, 3, 12)

// IntParmTemplate

class IntParmTemplate
{
public:
    void Hide(bool b) {}
    void SetConditional(ParmCondType type, const std::string& val) {}
    void SetTags(const dict& tags) {}

}; // IntParmTemplate

IntParmTemplate hou_create_int_parm_template(const std::string& name, const std::string& type, int value, const list& default_value = {},
    float min = 0, float max = 1, bool min_is_strict = false, bool max_is_strict = false, ParmNamingScheme naming_scheme = ParmNamingScheme::Base1,
    const list& menu_items = {}, const list& menu_labels = {}, const list& icon_names = {}, const std::string& item_generator_script = "",
    ScriptLanguage item_generator_script_language = ScriptLanguage::Python, MenuType menu_type = MenuType::Normal)
{
    return IntParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_int_parm_template_overloads, hou_create_int_parm_template, 3, 15)

//////////////////////////////////////////////////////////////////////////
// hou
//////////////////////////////////////////////////////////////////////////

std::shared_ptr<NodeProxy> hou_get_node(const std::string& path)
{
    auto node = EVAL->QueryNodeByPath(path);
    if (node) {
        return std::make_shared<NodeProxy>(node);
    } else {
        return nullptr;
    }
}

sm::vec2 hou_create_vec2(float x, float y)
{
    return sm::vec2(x, y);
}

Color hou_create_color(const list& rgb)
{
    Color col;
    col.r = boost::python::extract<float>(rgb[0]);
    col.g = boost::python::extract<float>(rgb[1]);
    col.b = boost::python::extract<float>(rgb[2]);
    return col;
}

Keyframe hou_create_keyframe()
{
    return Keyframe();
}

StringKeyframe hou_create_stringkeyframe()
{
    return StringKeyframe();
}

BOOST_PYTHON_MODULE(hou)
{
    // enums
    enum_<FolderType>("folderType")
        .value("Collapsible", FolderType::Collapsible)
        .value("Simple", FolderType::Simple)
        .value("Tabs", FolderType::Tabs)
        .value("RadioButtons", FolderType::RadioButtons)
        .value("MultiparmBlock", FolderType::MultiparmBlock)
        .value("ScrollingMultiparmBlock", FolderType::ScrollingMultiparmBlock)
        .value("TabbedMultiparmBlock", FolderType::TabbedMultiparmBlock)
        .value("ImportBlock", FolderType::ImportBlock)
    ;
    enum_<ExprLanguage>("exprLanguage")
        .value("Python", ExprLanguage::Python)
        .value("Hscript", ExprLanguage::Hscript)
    ;
    enum_<ScriptLanguage>("scriptLanguage")
        .value("Python", ScriptLanguage::Python)
        .value("Hscript", ScriptLanguage::Hscript)
    ;
    enum_<MenuType>("menuType")
        .value("Normal", MenuType::Normal)
        .value("Mini", MenuType::Mini)
        .value("StringReplace", MenuType::StringReplace)
        .value("StringToggle", MenuType::StringToggle)
    ;
    enum_<ParmLook>("parmLook")
        .value("Regular", ParmLook::Regular)
        .value("Logarithmic", ParmLook::Logarithmic)
        .value("Angle", ParmLook::Angle)
        .value("Vector", ParmLook::Vector)
        .value("ColorSquare", ParmLook::ColorSquare)
        .value("HueCircle", ParmLook::HueCircle)
        .value("CRGBAPlaneChooser", ParmLook::CRGBAPlaneChooser)
    ;
    enum_<ParmNamingScheme>("parmNamingScheme")
        .value("Base1", ParmNamingScheme::Base1)
        .value("XYZW", ParmNamingScheme::XYZW)
        .value("XYWH", ParmNamingScheme::XYWH)
        .value("UVW", ParmNamingScheme::UVW)
        .value("RGBA", ParmNamingScheme::RGBA)
        .value("MinMax", ParmNamingScheme::MinMax)
        .value("MaxMin", ParmNamingScheme::MaxMin)
        .value("StartEnd", ParmNamingScheme::StartEnd)
        .value("BeginEnd", ParmNamingScheme::BeginEnd)
    ;
    enum_<StringParmType>("stringParmType")
        .value("Regular", StringParmType::Regular)
        .value("FileReference", StringParmType::FileReference)
        .value("NodeReference", StringParmType::NodeReference)
        .value("NodeReferenceList", StringParmType::NodeReferenceList)
    ;
    enum_<ParmCondType>("parmCondType")
        .value("DisableWhen", ParmCondType::DisableWhen)
        .value("HideWhen", ParmCondType::HideWhen)
    ;

    // types
    void(Parm::*ParmSet0)(float)              = &Parm::Set;
    void(Parm::*ParmSet1)(const std::string&) = &Parm::Set;
    void(Parm::*ParmSetKeyframe0)(const Keyframe&)       = &Parm::SetKeyframe;
    void(Parm::*ParmSetKeyframe1)(const StringKeyframe&) = &Parm::SetKeyframe;
    class_<Parm>("Parm", init<const std::string&, const sop::NodePtr&>())
        .def("setKeyframe", ParmSetKeyframe0)
        .def("setKeyframe", ParmSetKeyframe1)
        .def("set", ParmSet0)
        .def("set", ParmSet1)
    ;
    class_<ParmTuple>("ParmTuple", init<const std::string&, const sop::NodePtr&>())
        .def("setAutoscope", &ParmTuple::SetAutoscope)
        .def("set", &ParmTuple::Set)
        .def("__getitem__", &ParmTuple::GetItem, return_value_policy<return_by_value>())
    ;

    class_<sm::vec2>("Vector2", init<float, float>());
    def("Vector2", hou_create_vec2);

    class_<Color>("Color");
    def("Color", hou_create_color);

    class_<Keyframe>("Keyframe")
        .def("setTime", &Keyframe::SetTime)
        .def("setExpression", &Keyframe::SetExpression)
        .def("interpretAccelAsRatio", &Keyframe::InterpretAccelAsRatio)
    ;
    def("Keyframe", hou_create_keyframe);

    class_<StringKeyframe>("StringKeyframe")
        .def("setTime", &StringKeyframe::SetTime)
        .def("setExpression", &StringKeyframe::SetExpression)
    ;
    def("StringKeyframe", hou_create_stringkeyframe);

    // node
    class_<NodeProxy>("NodeProxy", init<const sop::NodePtr&>())
        .def("createNode", &NodeProxy::CreateNode, hou_node_create_node_overloads(
            args("type", "name", "run_init_scripts", "load_contents", "exact_type_name")
        ))
        .def("createNetworkBox", &NodeProxy::CreateNetworkBox)
        .def("findNetworkBox", &NodeProxy::FindNetworkBox)
        .def("parent", &NodeProxy::GetParent)
        .def("node", &NodeProxy::GetChild)
        .def("move", &NodeProxy::Move)
        .def("setSelectableInViewport", &NodeProxy::SetSelectableInViewport)
        .def("showOrigin", &NodeProxy::ShowOrigin)
        .def("useXray", &NodeProxy::UseXray)
        .def("hide", &NodeProxy::Hide)
        .def("setSelected", &NodeProxy::SetSelected)
        .def("setParmTemplateGroup", &NodeProxy::SetParmTemplateGroup)
        .def("parmTuple", &NodeProxy::GetParmTuple)
        .def("setExpressionLanguage", &NodeProxy::SetExpressionLanguage)
        .def("setUserData", &NodeProxy::SetUserData)
        .def("bypass", &NodeProxy::Bypass)
        .def("setDisplayFlag", &NodeProxy::SetDisplayFlag)
        .def("setHighlightFlag", &NodeProxy::SetHighlightFlag)
        .def("setHardLocked", &NodeProxy::SetHardLocked)
        .def("setSoftLocked", &NodeProxy::SetSoftLocked)
        .def("setSelectableTemplateFlag", &NodeProxy::SetSelectableTemplateFlag)
        .def("setRenderFlag", &NodeProxy::SetRenderFlag)
        .def("setTemplateFlag", &NodeProxy::SetTemplateFlag)
        .def("setUnloadFlag", &NodeProxy::SetUnloadFlag)
        .def("setInput", &NodeProxy::SetInput, hou_node_set_input_overloads(
            args("input_index", "item_to_become_input", "output_index")
        ))
        .def("setPosition", &NodeProxy::SetPosition)
        .def("setSize", &NodeProxy::SetSize)
        .def("setMinimized", &NodeProxy::SetMinimized)
        .def("setAutoFit", &NodeProxy::SetAutoFit)
        .def("setComment", &NodeProxy::SetComment)
        .def("setColor", &NodeProxy::SetColor)
        .def("parm", &NodeProxy::GetParm)
        .def("indirectInputs", &NodeProxy::IndirectInputs)
    ;

    // params
    class_<ParmTemplateGroup>("ParmTemplateGroup")
        .def("append", &ParmTemplateGroup::Append)
    ;
    def("ParmTemplateGroup", hou_create_parm_template_group);

    void(FolderParmTemplate::*AddParmTemplate0)(const MenuParmTemplate&)   = &FolderParmTemplate::AddParmTemplate;
    void(FolderParmTemplate::*AddParmTemplate1)(const FloatParmTemplate&)  = &FolderParmTemplate::AddParmTemplate;
    void(FolderParmTemplate::*AddParmTemplate2)(const ToggleParmTemplate&) = &FolderParmTemplate::AddParmTemplate;
    void(FolderParmTemplate::*AddParmTemplate3)(const StringParmTemplate&) = &FolderParmTemplate::AddParmTemplate;
    void(FolderParmTemplate::*AddParmTemplate4)(const IntParmTemplate&)    = &FolderParmTemplate::AddParmTemplate;
    void(FolderParmTemplate::*AddParmTemplate5)(const FolderParmTemplate&) = &FolderParmTemplate::AddParmTemplate;

    class_<FolderParmTemplate>("FolderParmTemplate")
        .def("addParmTemplate", AddParmTemplate0)
        .def("addParmTemplate", AddParmTemplate1)
        .def("addParmTemplate", AddParmTemplate2)
        .def("addParmTemplate", AddParmTemplate3)
        .def("addParmTemplate", AddParmTemplate4)
        .def("addParmTemplate", AddParmTemplate5)
    ;
    def("FolderParmTemplate", hou_create_folder_parm_template, hou_create_folder_parm_template_overloads(
        args("name", "type", "folder_type", "default_value", "ends_tab_group")
    ));

    class_<MenuParmTemplate>("MenuParmTemplate")
        .def("setJoinWithNext", &MenuParmTemplate::SetJoinWithNext)
        .def("setTags", &MenuParmTemplate::SetTags)
        .def("hide", &MenuParmTemplate::Hide)
        .def("setHelp", &MenuParmTemplate::SetHelp)
    ;
    def("MenuParmTemplate", hou_create_menu_parm_template, hou_create_menu_parm_template_overloads(
        args("name", "type", "menu_items", "menu_labels", "default_value", "icon_names", "item_generator_script", "item_generator_script_language", "menu_type")
    ));

    class_<FloatParmTemplate>("FloatParmTemplate")
        .def("setTags", &FloatParmTemplate::SetTags)
        .def("hide", &FloatParmTemplate::Hide)
        .def("setConditional", &FloatParmTemplate::SetConditional)
    ;
    def("FloatParmTemplate", hou_create_float_parm_template, hou_create_float_parm_template_overloads(
        args("name", "type", "val", "default_value", "min", "max", "min_is_strict", "max_is_strict", "look", "naming_scheme")
    ));

    class_<ToggleParmTemplate>("ToggleParmTemplate")
        .def("setJoinWithNext", &ToggleParmTemplate::SetJoinWithNext)
        .def("setTags", &ToggleParmTemplate::SetTags)
        .def("setConditional", &ToggleParmTemplate::SetConditional)
        .def("hide", &ToggleParmTemplate::Hide)
    ;
    def("ToggleParmTemplate", hou_create_toggle_parm_template, hou_create_toggle_parm_template_overloads(
        args("name", "type", "default_value")
    ));

    class_<StringParmTemplate>("StringParmTemplate")
        .def("setConditional", &StringParmTemplate::SetConditional)
        .def("setTags", &StringParmTemplate::SetTags)
        .def("hide", &StringParmTemplate::Hide)
        .def("setHelp", &StringParmTemplate::SetHelp)
    ;
    def("StringParmTemplate", hou_create_string_parm_template, hou_create_string_parm_template_overloads(
        args("name", "type", "value", "default_value", "naming_scheme", "string_type", "menu_items", "menu_labels", "icon_names", "item_generator_script", "item_generator_script_language", "menu_type")
    ));

    class_<IntParmTemplate>("IntParmTemplate")
        .def("hide", &IntParmTemplate::Hide)
        .def("setConditional", &IntParmTemplate::SetConditional)
        .def("setTags", &IntParmTemplate::SetTags)
    ;
    def("IntParmTemplate", hou_create_int_parm_template, hou_create_int_parm_template_overloads(
        args("name", "type", "value", "default_value", "min", "max", "min_is_strict", "max_is_strict", "naming_scheme", "menu_items", "menu_labels", "icon_names", "item_generator_script", "item_generator_script_language", "menu_type")
    ));

    // hou
    def("node", hou_get_node);
}

}

namespace sop
{

PyLoader::PyLoader(Evaluator& eval)
    : m_eval(eval)
{
    static bool inited = false;
    if (!inited) 
    {
        Py_Initialize();

        PyImport_AppendInittab("hou", &inithou);
        PyRun_SimpleString(R"(
import hou
)");

        register_ptr_to_python<std::shared_ptr<NodeProxy>>();
        inited = true;
    }
}

PyLoader::~PyLoader()
{
//    Py_Finalize();
}

void PyLoader::RunFile(const std::string& filepath)
{
    PrepareContext();

    std::ifstream fin(filepath);
    if (fin.fail()) {
        fin.close();
        return;
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();

    PyRun_SimpleString(buffer.str().c_str());

    fin.close();
}

void PyLoader::PrepareContext()
{
    EVAL = &m_eval;
}

}