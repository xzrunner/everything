#pragma once

#include <string>

#include <boost/python.hpp>

namespace sop
{
namespace py
{

// enums

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

class MenuParmTemplate
{
public:
    void SetJoinWithNext(bool b) {}
    void SetTags(const boost::python::dict& tags) {}
    void Hide(bool b) {}
    void SetHelp(const std::string& help) {}

}; // MenuParmTemplate

class FloatParmTemplate
{
public:
    void SetTags(const boost::python::dict& tags) {}
    void Hide(bool b) {}
    void SetConditional(ParmCondType type, const std::string& val) {}

}; // FloatParmTemplate

class ToggleParmTemplate
{
public:
    void SetJoinWithNext(bool b) {}
    void SetTags(const boost::python::dict& tags) {}
    void SetConditional(ParmCondType type, const std::string& val) {}
    void Hide(bool b) {}

}; // ToggleParmTemplate

class StringParmTemplate
{
public:
    void SetConditional(ParmCondType type, const std::string& val) {}
    void SetTags(const boost::python::dict& tags) {}
    void Hide(bool b) {}
    void SetHelp(const std::string& help) {}

}; // StringParmTemplate

class IntParmTemplate
{
public:
    void Hide(bool b) {}
    void SetConditional(ParmCondType type, const std::string& val) {}
    void SetTags(const boost::python::dict& tags) {}

}; // IntParmTemplate

void BindParmTemplate();

}
}