// manipulate
#include "everything/node/Transform.h"
// primitive
#include "everything/node/Box.h"
#include "everything/node/Sphere.h"
#include "everything/node/Curve.h"

#define REGIST_NODE_TYPE(name)                                 \
	rttr::registration::class_<evt::node::name>("evt::"#name)  \
		.constructor<>()                                       \
	;

RTTR_REGISTRATION
{

// manipulate
REGIST_NODE_TYPE(Transform)

// primitive
REGIST_NODE_TYPE(Box)
REGIST_NODE_TYPE(Sphere)
REGIST_NODE_TYPE(Curve)

}

namespace evt
{

void regist_rttr()
{
}

}