// manipulate
#include "everything/node/Transform.h"
// polygon
#include "everything/node/Boolean.h"
#include "everything/node/Knife.h"
#include "everything/node/PolyExtrude.h"
// primitive
#include "everything/node/Box.h"
#include "everything/node/Sphere.h"
#include "everything/node/Curve.h"
// utility
#include "everything/node/Blast.h"
#include "everything/node/CopyToPoints.h"
#include "everything/node/GroupCreate.h"
#include "everything/node/Merge.h"

#define REGIST_NODE_TYPE(name)                                 \
	rttr::registration::class_<evt::node::name>("evt::"#name)  \
		.constructor<>()                                       \
	;

RTTR_REGISTRATION
{

// manipulate
REGIST_NODE_TYPE(Transform)

// polygon
REGIST_NODE_TYPE(Boolean)
REGIST_NODE_TYPE(Knife)
REGIST_NODE_TYPE(PolyExtrude)

// primitive
REGIST_NODE_TYPE(Box)
REGIST_NODE_TYPE(Sphere)
REGIST_NODE_TYPE(Curve)

// utility
REGIST_NODE_TYPE(Blast)
REGIST_NODE_TYPE(CopyToPoints)
REGIST_NODE_TYPE(GroupCreate)
REGIST_NODE_TYPE(Merge)

}

namespace evt
{

void regist_rttr()
{
}

}