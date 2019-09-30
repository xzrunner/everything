#include "sop/SOP.h"
#include "sop/VexFuncs.h"

#include <vexc/Type.h>
#include <vexc/BuildInFuncs.h>

namespace sop
{

CU_SINGLETON_DEFINITION(SOP);

extern void regist_rttr();

SOP::SOP()
{
	regist_rttr();

    // vexc
    vexc::SetupTypeSystem();
    vexc::SetupBuildInFuncs();
    SetupVexFuncs();
}

}