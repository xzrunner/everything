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
    BindCommonTypes();
    BindParmTemplate();

    BindParmValue();
    BindNodeProxy();

    register_ptr_to_python<std::shared_ptr<NodeProxy>>();
    def("node", hou_get_node);
}

}

namespace sop
{

PyLoader::PyLoader(Evaluator& eval)
    : m_eval(eval)
{
    static bool inited = false;
    PyImport_AppendInittab("hou", &inithou);

    if (!inited)
    {
        Py_Initialize();

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
hou_parent = None
hou_node = None
)");

    PrepareContext();

    std::ifstream fin(filepath);
    if (fin.fail()) {
        fin.close();
        return;
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();

    try {
        PyRun_SimpleString(buffer.str().c_str());
//        exec(buffer.str().c_str());
    } catch (...) {
        boost::python::handle_exception();
    }

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