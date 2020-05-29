#include "sop/SOP.h"
#include "sop/VexFuncs.h"

#include <cslang/Type.h>
#include <cslang/BuildInFuncs.h>

namespace sop
{

CU_SINGLETON_DEFINITION(SOP);

extern void regist_rttr();

SOP::SOP()
{
	regist_rttr();

    // cslang
    cslang::SetupTypeSystem();
    cslang::SetupBuildInFuncs();
    SetupVexFuncs();
}

}