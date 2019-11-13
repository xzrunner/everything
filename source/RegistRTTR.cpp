// base
#include "sop/node/Geometry.h"
// attribute
#include "sop/node/AttributeCreate.h"
#include "sop/node/AttributePromote.h"
#include "sop/node/AttributeTransfer.h"
#include "sop/node/AttributeWrangle.h"
#include "sop/node/Measure.h"
#include "sop/node/Sort.h"
// import
#include "sop/node/File.h"
#include "sop/node/ObjectMerge.h"
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
#include "sop/node/UVQuickShade.h"
#include "sop/node/UVTransform.h"
#include "sop/node/UVUnwrap.h"
// nurbs
#include "sop/node/Carve.h"
// polygon
#include "sop/node/Add.h"
#include "sop/node/Boolean.h"
#include "sop/node/Divide.h"
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
// primitive extern
#include "sop/node/Dungeon.h"
// rigging
#include "sop/node/Lattice.h"
// utility
#include "sop/node/Blast.h"
#include "sop/node/CopyToPoints.h"
#include "sop/node/ForeachPrimBegin.h"
#include "sop/node/ForeachPrimEnd.h"
#include "sop/node/Merge.h"
#include "sop/node/Null.h"
#include "sop/node/Output.h"
#include "sop/node/Python.h"
#include "sop/node/Split.h"
#include "sop/node/Switch.h"

#define REGIST_NODE_TYPE(type, name)                           \
	rttr::registration::class_<sop::node::type>("sop::"#name)  \
		.constructor<>()                                       \
	;

RTTR_REGISTRATION
{

// base
REGIST_NODE_TYPE(Geometry, geometry)

// attribute
REGIST_NODE_TYPE(AttributeCreate, attribcreate::2.0)
REGIST_NODE_TYPE(AttributePromote, attribpromote)
REGIST_NODE_TYPE(AttributeTransfer, attribtransfer)
REGIST_NODE_TYPE(AttributeWrangle, attribwrangle)
REGIST_NODE_TYPE(Measure, measure)
REGIST_NODE_TYPE(Sort, sort)

// group
REGIST_NODE_TYPE(GroupCreate, groupcreate)
REGIST_NODE_TYPE(GroupExpression, groupexpression)
REGIST_NODE_TYPE(GroupPromote, grouppromote)

// import
REGIST_NODE_TYPE(File, file)
REGIST_NODE_TYPE(ObjectMerge, object_merge)

// manipulate
REGIST_NODE_TYPE(Delete, delete)
REGIST_NODE_TYPE(Peak, peak)
REGIST_NODE_TYPE(Transform, xform)

// material
REGIST_NODE_TYPE(Color, color)
REGIST_NODE_TYPE(UVLayout, uvlayout)
REGIST_NODE_TYPE(UVQuickShade, uvquickshade)
REGIST_NODE_TYPE(UVTransform, uvtransform::2.0)
REGIST_NODE_TYPE(UVUnwrap, uvunwrap)

// NURBs
REGIST_NODE_TYPE(Carve, carve)

// polygon
REGIST_NODE_TYPE(Add, add)
REGIST_NODE_TYPE(Boolean, boolean::2.0)
REGIST_NODE_TYPE(Divide, divide)
REGIST_NODE_TYPE(Fuse, fuse)
REGIST_NODE_TYPE(Knife, knife)
REGIST_NODE_TYPE(Normal, normal)
REGIST_NODE_TYPE(PolyExtrude, polyextrude::2.0)
REGIST_NODE_TYPE(PolyFill, polyfill)
REGIST_NODE_TYPE(PolyFrame, polyframe)

// primitive
REGIST_NODE_TYPE(Box, box)
REGIST_NODE_TYPE(Curve, curve)
REGIST_NODE_TYPE(Grid, grid)
REGIST_NODE_TYPE(Line, line)
REGIST_NODE_TYPE(Primitive, primitive)
REGIST_NODE_TYPE(Sphere, sphere)
// primitive extern
REGIST_NODE_TYPE(Dungeon, dungeon)

// rigging
REGIST_NODE_TYPE(Lattice, lattice)

// utility
REGIST_NODE_TYPE(Blast, blast)
REGIST_NODE_TYPE(CopyToPoints, copytopoints)
REGIST_NODE_TYPE(ForeachPrimBegin, block_begin)
REGIST_NODE_TYPE(ForeachPrimEnd, block_end)
REGIST_NODE_TYPE(Merge, merge)
REGIST_NODE_TYPE(Null, null)
REGIST_NODE_TYPE(Output, output)
REGIST_NODE_TYPE(Python, python)
REGIST_NODE_TYPE(Split, split)
REGIST_NODE_TYPE(Switch, switch)

}

namespace sop
{

void regist_rttr()
{
}

}