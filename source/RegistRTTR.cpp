// manipulate
#include "everything/node/Transform.h"
// polygon
#include "everything/node/PolyExtrude.h"
// primitive
#include "everything/node/Box.h"
#include "everything/node/Sphere.h"
#include "everything/node/Curve.h"
// utility
#include "everything/node/Blast.h"
#include "everything/node/GroupCreate.h"

#define REGIST_NODE_TYPE(name)                                 \
	rttr::registration::class_<evt::node::name>("evt::"#name)  \
		.constructor<>()                                       \
	;

RTTR_REGISTRATION
{

// manipulate
REGIST_NODE_TYPE(Transform)

// polygon
REGIST_NODE_TYPE(PolyExtrude)

// primitive
REGIST_NODE_TYPE(Box)
REGIST_NODE_TYPE(Sphere)
REGIST_NODE_TYPE(Curve)

// utility
REGIST_NODE_TYPE(Blast)
REGIST_NODE_TYPE(GroupCreate)

}

namespace evt
{

void regist_rttr()
{
}

}