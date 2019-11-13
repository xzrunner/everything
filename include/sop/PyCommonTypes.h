#pragma once

#include "sop/typedef.h"

#include <string>

#include <boost/python.hpp>

namespace sop
{
namespace py
{

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

void BindCommonTypes();

}
}