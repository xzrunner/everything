// attribute
#include "everything/node/Sort.h"
// manipulate
#include "everything/node/Delete.h"
#include "everything/node/Transform.h"
// nurbs
#include "everything/node/Carve.h"
// polygon
#include "everything/node/Add.h"
#include "everything/node/Boolean.h"
#include "everything/node/Knife.h"
#include "everything/node/PolyExtrude.h"
// primitive
#include "everything/node/Box.h"
#include "everything/node/Curve.h"
#include "everything/node/Line.h"
#include "everything/node/Sphere.h"
// utility
#include "everything/node/Blast.h"
#include "everything/node/CopyToPoints.h"
#include "everything/node/GroupCreate.h"
#include "everything/node/Merge.h"
#include "everything/node/Switch.h"

#define REGIST_NODE_TYPE(name)                                 \
	rttr::registration::class_<evt::node::name>("evt::"#name)  \
		.constructor<>()                                       \
	;

RTTR_REGISTRATION
{

// attribute
REGIST_NODE_TYPE(Sort)

// manipulate
REGIST_NODE_TYPE(Delete)
REGIST_NODE_TYPE(Transform)

// NURBs
REGIST_NODE_TYPE(Carve)

// polygon
REGIST_NODE_TYPE(Add)
REGIST_NODE_TYPE(Boolean)
REGIST_NODE_TYPE(Knife)
REGIST_NODE_TYPE(PolyExtrude)

// primitive
REGIST_NODE_TYPE(Box)
REGIST_NODE_TYPE(Curve)
REGIST_NODE_TYPE(Line)
REGIST_NODE_TYPE(Sphere)

// utility
REGIST_NODE_TYPE(Blast)
REGIST_NODE_TYPE(CopyToPoints)
REGIST_NODE_TYPE(GroupCreate)
REGIST_NODE_TYPE(Merge)
REGIST_NODE_TYPE(Switch)

}

namespace evt
{

void regist_rttr()
{
}

}