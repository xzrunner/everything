#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyCommonTypes.h"

#include <boost/python.hpp>

using namespace boost::python;

namespace sop
{
namespace py
{

void BindCommonTypes()
{

    enum_<ExprLanguage>("exprLanguage")
        .value("Python", ExprLanguage::Python)
        .value("Hscript", ExprLanguage::Hscript)
    ;
    enum_<ScriptLanguage>("scriptLanguage")
        .value("Python", ScriptLanguage::Python)
        .value("Hscript", ScriptLanguage::Hscript)
    ;

}

}
}