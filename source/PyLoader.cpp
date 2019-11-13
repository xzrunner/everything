#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyLoader.h"
#include "sop/PyCommonTypes.h"
#include "sop/PyParmTemplate.h"
#include "sop/PyParmValue.h"
#include "sop/PyNodeProxy.h"
#include "sop/Evaluator.h"

#include <fstream>

#include <boost/python.hpp>

using namespace boost::python;
using namespace sop::py;

namespace
{

sop::Evaluator* EVAL = nullptr;

std::shared_ptr<NodeProxy> hou_get_node(const std::string& path)
{
    assert(EVAL);
    auto node = EVAL->QueryNodeByPath(path);
    if (node) {
        return std::make_shared<NodeProxy>(node);
    } else {
        return nullptr;
    }
}

BOOST_PYTHON_MODULE(hou)
{
    static bool binded = false;
    if (!binded) {
    BindCommonTypes();
    BindParmTemplate();

    BindParmValue();
    BindNodeProxy();

        binded = true;
    }
}

}

namespace sop
{

PyLoader::PyLoader(Evaluator& eval)
    : m_eval(eval)
{
    static bool inited = false;
    Py_Initialize();
    PyImport_AppendInittab("hou", &inithou);
//    Py_Initialize();

    if (!inited)
    {
//        PyImport_AppendInittab("hou", &inithou);
//        Py_Initialize();

        //import("hou");
//        PyRun_SimpleString(R"(
//import hou
//)");


        inited = true;
    }
}

PyLoader::~PyLoader()
{
//    Py_Finalize();
}

void PyLoader::RunFile(const std::string& filepath)
{
//    import("hou");
    PyRun_SimpleString(R"(
import hou
)");

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

//    PyRun_SimpleString(R"(
//import sys
//del sys.modules['hou']
//)");
    PyRun_SimpleString(R"(
del hou
)");
//    PyRun_SimpleString(R"(
//import sys
//sys.modules['hou'].__dict__.clear()
//)");
//    import("sys").attr("modules")["hou"].del();
}

void PyLoader::PrepareContext()
{
    NodeProxy::SetContext(&m_eval);
    EVAL = &m_eval;
}

}