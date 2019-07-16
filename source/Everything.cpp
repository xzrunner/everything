#include "everything/Everything.h"

namespace evt
{

CU_SINGLETON_DEFINITION(Everything);

extern void regist_rttr();

Everything::Everything()
{
	regist_rttr();
}

}