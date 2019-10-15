#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyLoader.h"
#include "sop/typedef.h"
#include "sop/Node.h"

#include <SM_Vector.h>

#include <fstream>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

namespace
{

//////////////////////////////////////////////////////////////////////////
// hou_node
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

class ParmTemplateGroup;

class ParmTuple
{
public:
    void SetAutoscope(const tuple& bool_values) {}
    void Set(const tuple& val) {}

}; // ParmTuple

class HouNode
{
public:
    HouNode() {}
    HouNode(const std::string& type, const std::string& name)
        : m_type(type)
        , m_name(name)
    {
    }

    std::shared_ptr<HouNode> CreateNode(const char* type, const char* name,
        bool run_init_scripts = false, bool load_contents = false, bool exact_type_name = false)
    {
        auto ret = std::make_shared<HouNode>(type, name);
        return ret;
    }

    std::shared_ptr<HouNode> Parent()
    {
        auto ret = std::make_shared<HouNode>();
        return ret;
    }
    std::shared_ptr<HouNode> Child(const std::string& path)
    {
        auto ret = std::make_shared<HouNode>();
        return ret;
    }

    void Move(const sm::vec2& v) {}
    void SetSelectableInViewport(bool b) {}
    void ShowOrigin(bool b) {}
    void UseXray(bool b) {}
    void Hide(bool b) {}
    void SetSelected(bool b) {}
    void SetParmTemplateGroup(const ParmTemplateGroup& parm) {}
    ParmTuple CreateParmTuple(const std::string& parm_path) { return ParmTuple(); }
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
    void SetInput(int input_index, const std::shared_ptr<HouNode>& item_to_become_input, int output_index = 0) {}

private:
    std::string m_type;
    std::string m_name;

    sop::NodePtr m_node = nullptr;

}; // HouNode

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_create_node_overloads, HouNode::CreateNode, 2, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_set_input_overloads, HouNode::SetInput, 2, 3)

BOOST_PYTHON_MODULE(hou_node)
{
    class_<ParmTuple>("ParmTuple")
        .def("setAutoscope", &ParmTuple::SetAutoscope)
        .def("set", &ParmTuple::Set)
    ;

    class_<HouNode>("HouNode")
        .def("createNode", &HouNode::CreateNode, hou_node_create_node_overloads(
            args("type", "name", "run_init_scripts", "load_contents", "exact_type_name")
        ))
        .def("parent", &HouNode::Parent)
        .def("node", &HouNode::Child)
        .def("move", &HouNode::Move)
        .def("setSelectableInViewport", &HouNode::SetSelectableInViewport)
        .def("showOrigin", &HouNode::ShowOrigin)
        .def("useXray", &HouNode::UseXray)
        .def("hide", &HouNode::Hide)
        .def("setSelected", &HouNode::SetSelected)
        .def("setParmTemplateGroup", &HouNode::SetParmTemplateGroup)
        .def("parmTuple", &HouNode::CreateParmTuple)
        .def("setExpressionLanguage", &HouNode::SetExpressionLanguage)
        .def("setUserData", &HouNode::SetUserData)
        .def("bypass", &HouNode::Bypass)
        .def("setDisplayFlag", &HouNode::SetDisplayFlag)
        .def("setHighlightFlag", &HouNode::SetHighlightFlag)
        .def("setHardLocked", &HouNode::SetHardLocked)
        .def("setSoftLocked", &HouNode::SetSoftLocked)
        .def("setSelectableTemplateFlag", &HouNode::SetSelectableTemplateFlag)
        .def("setRenderFlag", &HouNode::SetRenderFlag)
        .def("setTemplateFlag", &HouNode::SetTemplateFlag)
        .def("setUnloadFlag", &HouNode::SetUnloadFlag)
        .def("setInput", &HouNode::SetInput, hou_node_set_input_overloads(
            args("input_index", "item_to_become_input", "output_index")
        ))
    ;
}

//////////////////////////////////////////////////////////////////////////
// hou
//////////////////////////////////////////////////////////////////////////

std::shared_ptr<HouNode> hou_get_node(const char* path)
{
    return std::make_shared<HouNode>();
}

sm::vec2 hou_create_vec2(float x, float y)
{
    return sm::vec2(x, y);
}

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

enum class MenuType
{
    Normal,
    Mini,
    StringReplace,
    StringToggle,
};

class MenuParmTemplate
{
public:
    void SetJoinWithNext(bool b) {}
    void SetTags(const dict& tags) {}
    void Hide(bool b) {}
    void SetHelp(const std::string& help) {}

}; // MenuParmTemplate

MenuParmTemplate hou_create_menu_parm_template(const char* name, const char* type,
    const list& menu_items = {},
    const list& menu_labels = {},
    int default_value = 0, const list& icon_names = {}, const char* item_generator_script = "",
    ScriptLanguage item_generator_script_language = ScriptLanguage::Python, MenuType menu_type = MenuType::Normal)
{
    return MenuParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_menu_parm_template_overloads, hou_create_menu_parm_template, 2, 9)

// FloatParmTemplate

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

class FloatParmTemplate
{
public:
    void SetTags(const dict& tags) {}
    void Hide(bool b) {}
    void SetConditional(ParmCondType type, const std::string& val) {}

}; // FloatParmTemplate

FloatParmTemplate hou_create_float_parm_template(const char* name, const char* type, int val,
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

ToggleParmTemplate hou_create_toggle_parm_template(const char* name, const char* type, bool default_value = false)
{
    return ToggleParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_toggle_parm_template_overloads, hou_create_toggle_parm_template, 2, 3)

// StringParmTemplate

enum class StringParmType
{
    Regular,
    FileReference,
    NodeReference,
    NodeReferenceList,
};

class StringParmTemplate
{
public:
    void SetConditional(ParmCondType type, const std::string& val) {}
    void SetTags(const dict& tags) {}
    void Hide(bool b) {}
    void SetHelp(const std::string& help) {}

}; // StringParmTemplate

StringParmTemplate hou_create_string_parm_template(const char* name, const char* type, int value, const list& default_value = {},
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

IntParmTemplate hou_create_int_parm_template(const char* name, const char* type, int value, const list& default_value = {},
    float min = 0, float max = 1, bool min_is_strict = false, bool max_is_strict = false, ParmNamingScheme naming_scheme = ParmNamingScheme::Base1,
    const list& menu_items = {}, const list& menu_labels = {}, const list& icon_names = {}, const std::string& item_generator_script = "",
    ScriptLanguage item_generator_script_language = ScriptLanguage::Python, MenuType menu_type = MenuType::Normal)
{
    return IntParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_int_parm_template_overloads, hou_create_int_parm_template, 3, 15)

BOOST_PYTHON_MODULE(hou)
{
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

    def("node", hou_get_node);

    class_<sm::vec2>("Vector2", init<float, float>());
    def("Vector2", hou_create_vec2);

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
}

}

namespace sop
{

PyLoader::PyLoader()
{
    PyImport_AppendInittab("hou", &inithou);
    PyImport_AppendInittab("hou_node", &inithou_node);
    register_ptr_to_python<std::shared_ptr<HouNode>>();

    Py_Initialize();
}

PyLoader::~PyLoader()
{
    Py_Finalize();
}

void PyLoader::RunFile(const char* filepath)
{
    std::ifstream fin(filepath);
    if (fin.fail()) {
        fin.close();
        return;
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();

    std::string str = R"(
import hou
import hou_node
)";
    str += buffer.str();

    PyRun_SimpleString(str.c_str());

    fin.close();
}

}