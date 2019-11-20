// base
#include "sop/node/Geometry.h"
// attribute
#include "sop/node/AttributeCreate.h"
#include "sop/node/AttributePromote.h"
#include "sop/node/AttributeTransfer.h"
#include "sop/node/AttributeVOP.h"
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

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

// base

rttr::registration::class_<hdiop::NodePort<sop::NodeVarType>>("sop::Node::Port")
	.property("var", &hdiop::NodePort<sop::NodeVarType>::var)
;

rttr::registration::class_<sop::Node>("sop::Node")
	.method("GetImports", &sop::Node::GetImports)
	.method("GetExports", &sop::Node::GetExports)
;

rttr::registration::enumeration<sop::GroupType>("sop_group_type")
(
    REGIST_ENUM_ITEM(sop::GroupType::GuessFromGroup, "guess",     "Guess From Group"),
    REGIST_ENUM_ITEM(sop::GroupType::Points,         "point",     "Points"),
    REGIST_ENUM_ITEM(sop::GroupType::Vertices,       "vertex",    "Vertices"),
    REGIST_ENUM_ITEM(sop::GroupType::Edges,          "edge",      "Edges"),
    REGIST_ENUM_ITEM(sop::GroupType::Primitives,     "primitive", "Primitives")
);

rttr::registration::enumeration<sop::GroupMerge>("sop_group_merge")
(
    REGIST_ENUM_ITEM(sop::GroupMerge::Replace,   "replace",   "Replace"),
    REGIST_ENUM_ITEM(sop::GroupMerge::Union,     "union",     "Union"),
    REGIST_ENUM_ITEM(sop::GroupMerge::Intersect, "intersect", "Intersect"),
    REGIST_ENUM_ITEM(sop::GroupMerge::Subtract,  "subtract",  "Subtract")
);

rttr::registration::enumeration<sop::GeoAttrType>("sop_geo_attr_type")
(
    REGIST_ENUM_ITEM(sop::GeoAttrType::Int,     "int",     "Int"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Bool,    "bool",    "Bool"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Double,  "double",  "Double"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Float,   "float",   "Float"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Float2,  "float2",  "Float2"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Float3,  "float3",  "Float3"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Float4,  "float4",  "Float4"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::String,  "string",  "String"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Vector,  "vector",  "Vector"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Vector4, "vector4", "Vector4"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Matrix2, "matrix2", "Matrix2"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Matrix3, "matrix3", "Matrix3"),
    REGIST_ENUM_ITEM(sop::GeoAttrType::Matrix4, "matrix4", "Matrix4")
);

rttr::registration::enumeration<sop::GeoAttrClass>("sop_geo_attr_class")
(
    REGIST_ENUM_ITEM(sop::GeoAttrClass::Point,     "point",     "Point"),
    REGIST_ENUM_ITEM(sop::GeoAttrClass::Vertex,    "vertex",    "Vertex"),
    REGIST_ENUM_ITEM(sop::GeoAttrClass::Primitive, "primitive", "Primitive"),
    REGIST_ENUM_ITEM(sop::GeoAttrClass::Detail,    "detail",    "Detail")
);

REGIST_NODE_TYPE(Geometry, geometry)

// attribute
REGIST_NODE_TYPE(AttributeCreate, attribcreate::2.0)
rttr::registration::enumeration<sop::node::AttributeCreate::ItemType>("sop_attributecreate_item_type")
(
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemType::Float,        "float",       "Float"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemType::Integer,      "int",         "Integer"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemType::Vector,       "vector",      "Vector"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemType::String,       "index",       "String"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemType::FloatArray,   "floatarray",  "Float Array"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemType::IntegerArray, "intarray",    "Integer Array"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemType::StringArray,  "stringarray", "String Array")
);
rttr::registration::enumeration<sop::node::AttributeCreate::ItemFltInfo>("sop_attributecreate_item_flt_info")
(
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::Guess,             "guess",        "Guess"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::None,              "none",         "None"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::Position,          "point",        "Position"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::Vector,            "vector",       "Vector"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::Normal,            "normal",       "Normal"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::Color,             "color",        "Color"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::Quaternion,        "quaternion",   "Quaternion"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::TransformMatrix,   "tranform",     "Transform Matrix"),
    REGIST_ENUM_ITEM(sop::node::AttributeCreate::ItemFltInfo::TextureCoordinate, "texturecoord", "Texture Coordinate")
);
REGIST_NODE_TYPE(AttributePromote, attribpromote)
REGIST_NODE_TYPE(AttributeTransfer, attribtransfer)
REGIST_NODE_TYPE(AttributeVOP, attribvop)
REGIST_NODE_TYPE(AttributeWrangle, attribwrangle)
REGIST_NODE_TYPE(Measure, measure)
rttr::registration::enumeration<sop::node::Measure::Type>("sop_measure_type")
(
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Perimeter, "perimeter", "Perimeter"),
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Area,      "area",      "Area"),
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Curvature, "curvature", "Curvature"),
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Volume,    "volume",    "Volume")
);
REGIST_NODE_TYPE(Sort, sort)
rttr::registration::enumeration<sop::node::Sort::Key>("sop_sort_key")
(
    REGIST_ENUM_ITEM(sop::node::Sort::Key::NoChange, "none", "No Change"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::X,        "byx",  "X"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::Y,        "byy",  "Y"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::Z,        "byz",  "Z"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::Shift,    "shif", "Shift")
);

// group
REGIST_NODE_TYPE(GroupCreate, groupcreate)
rttr::registration::enumeration<sop::node::GroupCreate::BoundingType>("sop_groupcreate_bounding_type")
(
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Box,    "usebbox",    "Box"),
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Sphere, "usebsphere", "Sphere"),
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Object, "usebobject", "Object"),
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Volume, "usebvolume", "Volume")
);
REGIST_NODE_TYPE(GroupExpression, groupexpression)
REGIST_NODE_TYPE(GroupPromote, grouppromote)

// import
REGIST_NODE_TYPE(File, file)
REGIST_NODE_TYPE(ObjectMerge, object_merge)

// manipulate
REGIST_NODE_TYPE(Delete, delete)
rttr::registration::enumeration<sop::node::Delete::EntityType>("sop_delete_entity_type")
(
    REGIST_ENUM_ITEM(sop::node::Delete::EntityType::Points, "point", "Points"),
    REGIST_ENUM_ITEM(sop::node::Delete::EntityType::Edges,  "edge",  "Edges"),
    REGIST_ENUM_ITEM(sop::node::Delete::EntityType::Faces,  "face",  "Faces")
);
rttr::registration::enumeration<sop::node::Delete::Operation>("sop_delete_operation")
(
    REGIST_ENUM_ITEM(sop::node::Delete::Operation::Pattern,    "pattern", "Pattern"),
    REGIST_ENUM_ITEM(sop::node::Delete::Operation::Range,      "range",   "Range"),
    REGIST_ENUM_ITEM(sop::node::Delete::Operation::Expression, "filter",  "Expression")
);
REGIST_NODE_TYPE(Peak, peak)
REGIST_NODE_TYPE(Transform, xform)

// material
REGIST_NODE_TYPE(Color, color)
REGIST_NODE_TYPE(UVLayout, uvlayout)
REGIST_NODE_TYPE(UVQuickShade, uvquickshade)
REGIST_NODE_TYPE(UVTransform, uvtransform::2.0)
REGIST_NODE_TYPE(UVUnwrap, uvunwrap)
rttr::registration::enumeration<sop::node::UVUnwrap::ScaleType>("sop_uvunwrap_scale_type")
(
    REGIST_ENUM_ITEM(sop::node::UVUnwrap::ScaleType::None,    "none",    "None"),
    REGIST_ENUM_ITEM(sop::node::UVUnwrap::ScaleType::Uniform, "uniform", "Uniform"),
    REGIST_ENUM_ITEM(sop::node::UVUnwrap::ScaleType::Stretch, "stretch", "Stretch")
);

// NURBs
REGIST_NODE_TYPE(Carve, carve)

// polygon
REGIST_NODE_TYPE(Add, add)
REGIST_NODE_TYPE(Boolean, boolean::2.0)
rttr::registration::enumeration<sop::node::Boolean::Operator>("sop_boolean_operator")
(
    REGIST_ENUM_ITEM(sop::node::Boolean::Operator::Union,     "union",     "Union"),
    REGIST_ENUM_ITEM(sop::node::Boolean::Operator::Intersect, "intersect", "Intersect"),
    REGIST_ENUM_ITEM(sop::node::Boolean::Operator::Subtract,  "subtract",  "Subtract")
);
rttr::registration::enumeration<sop::node::Boolean::GeoType>("sop_boolean_geo_type")
(
    REGIST_ENUM_ITEM(sop::node::Boolean::GeoType::Solid,   "solid",   "Solid"),
    REGIST_ENUM_ITEM(sop::node::Boolean::GeoType::Surface, "surface", "Surface")
);
rttr::registration::enumeration<sop::node::Boolean::SubType>("sop_boolean_sub_type")
(
    REGIST_ENUM_ITEM(sop::node::Boolean::SubType::AMinusB, "aminusb", "A - B"),
    REGIST_ENUM_ITEM(sop::node::Boolean::SubType::BMinusA, "bminusa", "B - A"),
    REGIST_ENUM_ITEM(sop::node::Boolean::SubType::Both,    "both",    "Both")
);
REGIST_NODE_TYPE(Divide, divide)
REGIST_NODE_TYPE(Fuse, fuse)
rttr::registration::enumeration<sop::node::Fuse::Operator>("sop_fuse_operator")
(
    REGIST_ENUM_ITEM(sop::node::Fuse::Operator::Consolidate,  "consolidate",   "Consolidate"),
    REGIST_ENUM_ITEM(sop::node::Fuse::Operator::UniquePoints, "unique_points", "Unique Points"),
    REGIST_ENUM_ITEM(sop::node::Fuse::Operator::Snap,         "snap",          "Snap")
);
REGIST_NODE_TYPE(Knife, knife)
rttr::registration::enumeration<sop::node::Knife::KeepType>("sop_knife_keep")
(
    REGIST_ENUM_ITEM(sop::node::Knife::KeepType::KeepAbove, "above", "Keep Above"),
    REGIST_ENUM_ITEM(sop::node::Knife::KeepType::KeepBelow, "below", "Keep Below"),
    REGIST_ENUM_ITEM(sop::node::Knife::KeepType::KeepAll,   "both",  "Keep All")
);
REGIST_NODE_TYPE(Normal, normal)
REGIST_NODE_TYPE(PolyExtrude, polyextrude::2.0)
rttr::registration::enumeration<sop::node::PolyExtrude::Extrusion>("sop_polyextrude_extrusion")
(
    REGIST_ENUM_ITEM(sop::node::PolyExtrude::Extrusion::PrimEdgeNorm, "primnormal", "Prim Edge Norm"),
    REGIST_ENUM_ITEM(sop::node::PolyExtrude::Extrusion::PointNorm,    "pointnormal", "Point Norm")
);
REGIST_NODE_TYPE(PolyFill, polyfill)
rttr::registration::enumeration<sop::node::PolyFill::Mode>("sop_polyfill_mode")
(
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::SinglePolygon,     "none",      "Single Polygon"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::Triangles,         "tris",      "Triangles"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::TriangleFan,       "trifan",    "Triangle Fan"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::QuadrilateralFan,  "quadfan",   "Quadrilateral Fan"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::Quadrilaterals,    "quads",     "Quadrilaterals"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::QuadrilateralGrid, "gridquads", "Quadrilateral Grid")
);
REGIST_NODE_TYPE(PolyFrame, polyframe)
rttr::registration::enumeration<sop::node::PolyFrame::FrameStyle>("sop_poly_frame_style")
(
    REGIST_ENUM_ITEM(sop::node::PolyFrame::FrameStyle::FirstEdge,         "first_edge",          "First Edge"),
    REGIST_ENUM_ITEM(sop::node::PolyFrame::FrameStyle::TwoEdges,          "two_edges",           "Two Edges"),
    REGIST_ENUM_ITEM(sop::node::PolyFrame::FrameStyle::PrimitiveCentroid, "primitive_centroid",  "Primitive Centroid"),
    REGIST_ENUM_ITEM(sop::node::PolyFrame::FrameStyle::TextureUV,         "texture_uv",          "TextureUV"),
    REGIST_ENUM_ITEM(sop::node::PolyFrame::FrameStyle::TextureUVGradient, "texture_uv_gradient", "TextureUV Gradient"),
    REGIST_ENUM_ITEM(sop::node::PolyFrame::FrameStyle::AttributeGradient, "attribute_gradient",  "Attribute Gradient"),
    REGIST_ENUM_ITEM(sop::node::PolyFrame::FrameStyle::MikkT,             "mikkt",               "MikkT")
);

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
rttr::registration::enumeration<sop::node::ForeachPrimBegin::Method>("sop_foreachprimbegin_method")
(
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::Feedback,     "feedback", "Feedback"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::PieceOrPoint, "piece",    "Piece or Point"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::Metadata,     "metadata", "Metadata"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::Input,        "input",    "Input")
);
REGIST_NODE_TYPE(ForeachPrimEnd, block_end)
rttr::registration::enumeration<sop::node::ForeachPrimEnd::IterMethod>("sop_foreachprimend_iter_method")
(
    REGIST_ENUM_ITEM(sop::node::ForeachPrimEnd::IterMethod::AutoDetectFromInputs, "auto",   "Auto Detect from Inputs"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimEnd::IterMethod::ByPiecesOrPoints,     "pieces", "By Pieces or Points"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimEnd::IterMethod::ByCount,              "count",  "By Count")
);
rttr::registration::enumeration<sop::node::ForeachPrimEnd::GatherMethod>("sop_foreachprimend_gather_method")
(
    REGIST_ENUM_ITEM(sop::node::ForeachPrimEnd::GatherMethod::Feedback, "feedback", "Feedback"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimEnd::GatherMethod::Merge,    "merge",    "Merge")
);
rttr::registration::enumeration<sop::node::ForeachPrimEnd::PieceElements>("sop_foreachprimend_piece_elements")
(
    REGIST_ENUM_ITEM(sop::node::ForeachPrimEnd::PieceElements::Primitives, "primitive", "Primitives"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimEnd::PieceElements::Points,     "point",     "Points")
);
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