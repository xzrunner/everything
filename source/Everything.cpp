#include "everything/Everything.h"
#include "everything/VexFuncs.h"

#include <vexc/Type.h>
#include <vexc/BuildInFuncs.h>

namespace evt
{

CU_SINGLETON_DEFINITION(Everything);

extern void regist_rttr();

Everything::Everything()
{
	regist_rttr();

    // vexc
    vexc::SetupTypeSystem();
    vexc::SetupBuildInFuncs();
    SetupVexFuncs();
}

}