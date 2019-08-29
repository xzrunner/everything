#include "everything/VexFuncs.h"

#include <vexc/EvalAST.h>
#include <vexc/StringPool.h>
#include <cpputil/StringHelper.h>

namespace evt
{

void SetupVexFuncs()
{
    vexc::RegistBuildInFunc("ch", [](const std::vector<vexc::Variant>& params)->vexc::Variant
    {
        if (params.empty()) {
            return vexc::Variant(false);
        }

        auto& p = params[0];
        if (p.type != vexc::VarType::String) {
            return vexc::Variant(false);
        }

        std::string path(vexc::StringPool::VoidToString(p.p));
        std::vector<std::string> tokens;
        cpputil::StringHelper::Split(path, "/", tokens);
//        int curr_level =
        for (auto& t : tokens)
        {

        }
    });
}

}