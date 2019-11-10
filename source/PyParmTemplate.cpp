#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyParmTemplate.h"
#include "sop/PyCommonTypes.h"

#include <boost/python.hpp>

using namespace boost::python;
using namespace sop::py;

namespace
{

// FolderParmTemplate

FolderParmTemplate hou_create_folder_parm_template(const std::string& name, const std::string& type,
    FolderType folder_type = FolderType::Tabs, int default_value = 0, bool ends_tab_group = false)
{
    return FolderParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_folder_parm_template_overloads, hou_create_folder_parm_template, 2, 5)


// MenuParmTemplate

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

FloatParmTemplate hou_create_float_parm_template(const std::string& name, const std::string& type, int val,
    const list& default_value = {}, float min = 0, float max = 360, bool min_is_strict = false, bool max_is_strict = false,
    ParmLook look = ParmLook::Regular, ParmNamingScheme naming_scheme = ParmNamingScheme::XYZW)
{
    return FloatParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_float_parm_template_overloads, hou_create_float_parm_template, 3, 10)

// ToggleParmTemplate

ToggleParmTemplate hou_create_toggle_parm_template(const std::string& name, const std::string& type, bool default_value = false)
{
    return ToggleParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_toggle_parm_template_overloads, hou_create_toggle_parm_template, 2, 3)

// StringParmTemplate

StringParmTemplate hou_create_string_parm_template(const std::string& name, const std::string& type, int value, const list& default_value = {},
    ParmNamingScheme naming_scheme = ParmNamingScheme::Base1, StringParmType string_type = StringParmType::NodeReference,
    const list& menu_items = {}, const list& menu_labels = {}, const list& icon_names = {}, const std::string& item_generator_script = "",
    ScriptLanguage item_generator_script_language = ScriptLanguage::Python, MenuType menu_type = MenuType::Normal)
{
    return StringParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_string_parm_template_overloads, hou_create_string_parm_template, 3, 12)

// IntParmTemplate

IntParmTemplate hou_create_int_parm_template(const std::string& name, const std::string& type, int value, const list& default_value = {},
    float min = 0, float max = 1, bool min_is_strict = false, bool max_is_strict = false, ParmNamingScheme naming_scheme = ParmNamingScheme::Base1,
    const list& menu_items = {}, const list& menu_labels = {}, const list& icon_names = {}, const std::string& item_generator_script = "",
    ScriptLanguage item_generator_script_language = ScriptLanguage::Python, MenuType menu_type = MenuType::Normal)
{
    return IntParmTemplate();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(hou_create_int_parm_template_overloads, hou_create_int_parm_template, 3, 15)

}

namespace sop
{
namespace py
{

void BindParmTemplate()
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
    enum_<MenuType>("menuType")
        .value("Normal", MenuType::Normal)
        .value("Mini", MenuType::Mini)
        .value("StringReplace", MenuType::StringReplace)
        .value("StringToggle", MenuType::StringToggle)
    ;
    enum_<StringParmType>("stringParmType")
        .value("Regular", StringParmType::Regular)
        .value("FileReference", StringParmType::FileReference)
        .value("NodeReference", StringParmType::NodeReference)
        .value("NodeReferenceList", StringParmType::NodeReferenceList)
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
    enum_<ParmCondType>("parmCondType")
        .value("DisableWhen", ParmCondType::DisableWhen)
        .value("HideWhen", ParmCondType::HideWhen)
    ;

    // FolderParmTemplate
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

    // MenuParmTemplate
    class_<MenuParmTemplate>("MenuParmTemplate")
        .def("setJoinWithNext", &MenuParmTemplate::SetJoinWithNext)
        .def("setTags", &MenuParmTemplate::SetTags)
        .def("hide", &MenuParmTemplate::Hide)
        .def("setHelp", &MenuParmTemplate::SetHelp)
    ;
    def("MenuParmTemplate", hou_create_menu_parm_template, hou_create_menu_parm_template_overloads(
        args("name", "type", "menu_items", "menu_labels", "default_value", "icon_names", "item_generator_script", "item_generator_script_language", "menu_type")
    ));

    // FloatParmTemplate
    class_<FloatParmTemplate>("FloatParmTemplate")
        .def("setTags", &FloatParmTemplate::SetTags)
        .def("hide", &FloatParmTemplate::Hide)
        .def("setConditional", &FloatParmTemplate::SetConditional)
    ;
    def("FloatParmTemplate", hou_create_float_parm_template, hou_create_float_parm_template_overloads(
        args("name", "type", "val", "default_value", "min", "max", "min_is_strict", "max_is_strict", "look", "naming_scheme")
    ));

    // ToggleParmTemplate
    class_<ToggleParmTemplate>("ToggleParmTemplate")
        .def("setJoinWithNext", &ToggleParmTemplate::SetJoinWithNext)
        .def("setTags", &ToggleParmTemplate::SetTags)
        .def("setConditional", &ToggleParmTemplate::SetConditional)
        .def("hide", &ToggleParmTemplate::Hide)
    ;
    def("ToggleParmTemplate", hou_create_toggle_parm_template, hou_create_toggle_parm_template_overloads(
        args("name", "type", "default_value")
    ));

    // StringParmTemplate
    class_<StringParmTemplate>("StringParmTemplate")
        .def("setConditional", &StringParmTemplate::SetConditional)
        .def("setTags", &StringParmTemplate::SetTags)
        .def("hide", &StringParmTemplate::Hide)
        .def("setHelp", &StringParmTemplate::SetHelp)
    ;
    def("StringParmTemplate", hou_create_string_parm_template, hou_create_string_parm_template_overloads(
        args("name", "type", "value", "default_value", "naming_scheme", "string_type", "menu_items", "menu_labels", "icon_names", "item_generator_script", "item_generator_script_language", "menu_type")
    ));

    // IntParmTemplate
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
}