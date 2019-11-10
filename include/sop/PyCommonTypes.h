#pragma once

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

void BindCommonTypes();

}
}