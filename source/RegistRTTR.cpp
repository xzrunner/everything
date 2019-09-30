// base
#include "everything/node/Geometry.h"
// attribute
#include "everything/node/AttributeWrangle.h"
#include "everything/node/Measure.h"
#include "everything/node/Sort.h"
// group
#include "everything/node/GroupCreate.h"
#include "everything/node/GroupExpression.h"
// manipulate
#include "everything/node/Delete.h"
#include "everything/node/Transform.h"
// material
#include "everything/node/Color.h"
// nurbs
#include "everything/node/Carve.h"
// polygon
#include "everything/node/Add.h"
#include "everything/node/Boolean.h"
#include "everything/node/Fuse.h"
#include "everything/node/Knife.h"
#include "everything/node/Normal.h"
#include "everything/node/PolyExtrude.h"
#include "everything/node/PolyFill.h"
// primitive
#include "everything/node/Box.h"
#include "everything/node/Curve.h"
#include "everything/node/Line.h"
#include "everything/node/Primitive.h"
#include "everything/node/Sphere.h"
// utility
#include "everything/node/Blast.h"
#include "everything/node/CopyToPoints.h"
#include "everything/node/ForeachPrimBegin.h"
#include "everything/node/ForeachPrimEnd.h"
#include "everything/node/Merge.h"
#include "everything/node/Null.h"
#include "everything/node/Switch.h"

#define REGIST_NODE_TYPE(name)                                 \
	rttr::registration::class_<evt::node::name>("evt::"#name)  \
		.constructor<>()                                       \
	;

RTTR_REGISTRATION
{

// base
REGIST_NODE_TYPE(Geometry)

// attribute
REGIST_NODE_TYPE(AttributeWrangle)
REGIST_NODE_TYPE(Measure)
REGIST_NODE_TYPE(Sort)

// group
REGIST_NODE_TYPE(GroupCreate)
REGIST_NODE_TYPE(GroupExpression)

// manipulate
REGIST_NODE_TYPE(Delete)
REGIST_NODE_TYPE(Transform)

// material
REGIST_NODE_TYPE(Color)

// NURBs
REGIST_NODE_TYPE(Carve)

// polygon
REGIST_NODE_TYPE(Add)
REGIST_NODE_TYPE(Boolean)
REGIST_NODE_TYPE(Fuse)
REGIST_NODE_TYPE(Knife)
REGIST_NODE_TYPE(Normal)
REGIST_NODE_TYPE(PolyExtrude)
REGIST_NODE_TYPE(PolyFill)

// primitive
REGIST_NODE_TYPE(Box)
REGIST_NODE_TYPE(Curve)
REGIST_NODE_TYPE(Line)
REGIST_NODE_TYPE(Primitive)
REGIST_NODE_TYPE(Sphere)

// utility
REGIST_NODE_TYPE(Blast)
REGIST_NODE_TYPE(CopyToPoints)
REGIST_NODE_TYPE(ForeachPrimBegin)
REGIST_NODE_TYPE(ForeachPrimEnd)
REGIST_NODE_TYPE(Merge)
REGIST_NODE_TYPE(Null)
REGIST_NODE_TYPE(Switch)

}

namespace evt
{

void regist_rttr()
{
}

}