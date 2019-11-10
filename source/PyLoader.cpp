#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sop/PyLoader.h"
#include "sop/PyCommonTypes.h"
#include "sop/PyParmTemplate.h"
#include "sop/PyNode.h"
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
    auto node = EVAL->QueryNodeByPath(path);
    if (node) {
        return std::make_shared<NodeProxy>(node, *EVAL);
    } else {
        return nullptr;
    }
}

BOOST_PYTHON_MODULE(hou)
{
    BindCommonTypes();
    BindParmTemplate();
    BindNode();

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