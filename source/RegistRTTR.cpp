// base
#include "sop/node/Geometry.h"
// attribute
#include "sop/node/AttributeCreate.h"
#include "sop/node/AttributePromote.h"
#include "sop/node/AttributeTransfer.h"
#include "sop/node/AttributeWrangle.h"
#include "sop/node/Measure.h"
#include "sop/node/Sort.h"
// export
#include "sop/node/File.h"
// group
#include "sop/node/GroupCreate.h"
#include "sop/node/GroupExpression.h"
#include "sop/node/GroupPromote.h"
// manipulate
#include "sop/node/Delete.h"
#include "sop/node/Peak.h"
#include "sop/node/Transform.h"
// material
#include "sop/node/Color.h"
#include "sop/node/UVLayout.h"
#include "sop/node/UVUnwrap.h"
// nurbs
#include "sop/node/Carve.h"
// polygon
#include "sop/node/Add.h"
#include "sop/node/Boolean.h"
#include "sop/node/Fuse.h"
#include "sop/node/Knife.h"
#include "sop/node/Normal.h"
#include "sop/node/PolyExtrude.h"
#include "sop/node/PolyFill.h"
#include "sop/node/PolyFrame.h"
// primitive
#include "sop/node/Box.h"
#include "sop/node/Curve.h"
#include "sop/node/Grid.h"
#include "sop/node/Line.h"
#include "sop/node/Primitive.h"
#include "sop/node/Sphere.h"
// utility
#include "sop/node/Blast.h"
#include "sop/node/CopyToPoints.h"
#include "sop/node/ForeachPrimBegin.h"
#include "sop/node/ForeachPrimEnd.h"
#include "sop/node/Merge.h"
#include "sop/node/Null.h"
#include "sop/node/Output.h"
#include "sop/node/Split.h"
#include "sop/node/Switch.h"

#define REGIST_NODE_TYPE(name)                                 \
	rttr::registration::class_<sop::node::name>("sop::"#name)  \
		.constructor<>()                                       \
	;

RTTR_REGISTRATION
{

// base
REGIST_NODE_TYPE(Geometry)

// attribute
REGIST_NODE_TYPE(AttributeCreate)
REGIST_NODE_TYPE(AttributePromote)
REGIST_NODE_TYPE(AttributeTransfer)
REGIST_NODE_TYPE(AttributeWrangle)
REGIST_NODE_TYPE(Measure)
REGIST_NODE_TYPE(Sort)

// export
REGIST_NODE_TYPE(File)

// group
REGIST_NODE_TYPE(GroupCreate)
REGIST_NODE_TYPE(GroupExpression)
REGIST_NODE_TYPE(GroupPromote)

// manipulate
REGIST_NODE_TYPE(Delete)
REGIST_NODE_TYPE(Peak)
REGIST_NODE_TYPE(Transform)

// material
REGIST_NODE_TYPE(Color)
REGIST_NODE_TYPE(UVLayout)
REGIST_NODE_TYPE(UVUnwrap)

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
REGIST_NODE_TYPE(PolyFrame)

// primitive
REGIST_NODE_TYPE(Box)
REGIST_NODE_TYPE(Curve)
REGIST_NODE_TYPE(Grid)
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
REGIST_NODE_TYPE(Output)
REGIST_NODE_TYPE(Split)
REGIST_NODE_TYPE(Switch)

}

namespace sop
{

void regist_rttr()
{
}

}