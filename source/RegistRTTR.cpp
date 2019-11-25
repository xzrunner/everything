#include "sop/RTTRInfo.h"

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

rttr::registration::enumeration<sop::GroupTypes>("sop_group_types")
(
    REGIST_ENUM_ITEM(sop::GroupTypes::Auto,       "auto",     "Auto"),
    REGIST_ENUM_ITEM(sop::GroupTypes::Points,     "points",   "Points"),
    REGIST_ENUM_ITEM(sop::GroupTypes::Vertices,   "vertices", "Vertices"),
    REGIST_ENUM_ITEM(sop::GroupTypes::Edges,      "edges",    "Edges"),
    REGIST_ENUM_ITEM(sop::GroupTypes::Primitives, "prims",    "Primitives")
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

// widgets

rttr::registration::enumeration<sop::InterpType>("sop_interp_type")
(
    REGIST_ENUM_ITEM(sop::InterpType::Constant,      "constant",      "Constant"),
    REGIST_ENUM_ITEM(sop::InterpType::Linear,        "linear",        "Constant"),
    REGIST_ENUM_ITEM(sop::InterpType::Catmull_Rom,   "catmull-rom",   "Catmull-Rom"),
    REGIST_ENUM_ITEM(sop::InterpType::MonotoneCubic, "monotonecubic", "Monotone Cubic"),
    REGIST_ENUM_ITEM(sop::InterpType::Bezier,        "bezier",        "Bezier"),
    REGIST_ENUM_ITEM(sop::InterpType::B_Spline,      "bspline",       "B-Spline"),
    REGIST_ENUM_ITEM(sop::InterpType::Hermite,       "hermite",       "Hermite")
);

rttr::registration::class_<sop::RampFloat>("sop::ramp_float")
	.property("value",  &sop::RampFloat::value)
    .property("interp", &sop::RampFloat::interp)
    .property("pos",    &sop::RampFloat::pos)
;

rttr::registration::class_<sop::RampColor>("sop::ramp_color")
	.property("c",      &sop::RampColor::color)
    .property("interp", &sop::RampColor::interp)
    .property("pos",    &sop::RampColor::pos)
;

// attribute
rttr::registration::class_<sop::node::AttributeCreate>("sop::attribcreate::2.0")
.constructor<>()
#define PARM_FILEPATH "sop/node/AttributeCreate.parm.h"
#define PARM_NODE_CLASS sop::node::AttributeCreate
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
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
rttr::registration::class_<sop::node::AttributePromote>("sop::attribpromote")
.constructor<>()
#define PARM_FILEPATH "sop/node/AttributePromote.parm.h"
#define PARM_NODE_CLASS sop::node::AttributePromote
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::AttributeTransfer>("sop::attribtransfer")
.constructor<>()
#define PARM_FILEPATH "sop/node/AttributeTransfer.parm.h"
#define PARM_NODE_CLASS sop::node::AttributeTransfer
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
REGIST_NODE_TYPE(AttributeVOP, attribvop)
rttr::registration::class_<sop::node::AttributeWrangle>("sop::attribwrangle")
.constructor<>()
#define PARM_FILEPATH "sop/node/AttributeWrangle.parm.h"
#define PARM_NODE_CLASS sop::node::AttributeWrangle
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Measure>("sop::measure")
.constructor<>()
#define PARM_FILEPATH "sop/node/Measure.parm.h"
#define PARM_NODE_CLASS sop::node::Measure
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::Measure::Type>("sop_measure_type")
(
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Perimeter, "perimeter", "Perimeter"),
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Area,      "area",      "Area"),
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Curvature, "curvature", "Curvature"),
    REGIST_ENUM_ITEM(sop::node::Measure::Type::Volume,    "volume",    "Volume")
);
rttr::registration::class_<sop::node::Sort>("sop::sort")
.constructor<>()
#define PARM_FILEPATH "sop/node/Sort.parm.h"
#define PARM_NODE_CLASS sop::node::Sort
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::Sort::Key>("sop_sort_key")
(
    REGIST_ENUM_ITEM(sop::node::Sort::Key::NoChange, "none", "No Change"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::X,        "byx",  "X"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::Y,        "byy",  "Y"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::Z,        "byz",  "Z"),
    REGIST_ENUM_ITEM(sop::node::Sort::Key::Shift,    "shif", "Shift")
);

// group
rttr::registration::class_<sop::node::GroupCreate>("sop::groupcreate")
.constructor<>()
#define PARM_FILEPATH "sop/node/GroupCreate.parm.h"
#define PARM_NODE_CLASS sop::node::GroupCreate
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::GroupCreate::BoundingType>("sop_groupcreate_bounding_type")
(
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Box,    "usebbox",    "Box"),
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Sphere, "usebsphere", "Sphere"),
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Object, "usebobject", "Object"),
    REGIST_ENUM_ITEM(sop::node::GroupCreate::BoundingType::Volume, "usebvolume", "Volume")
);
rttr::registration::class_<sop::node::GroupExpression>("sop::groupexpression")
.constructor<>()
.property("expressions", &sop::node::GroupExpression::GetExprsNum, &sop::node::GroupExpression::SetExprsNum)
#define PARM_FILEPATH "sop/node/GroupExpression.parm.h"
#define PARM_NODE_CLASS sop::node::GroupExpression
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::GroupPromote>("sop::grouppromote")
.constructor<>()
#define PARM_FILEPATH "sop/node/GroupPromote.parm.h"
#define PARM_NODE_CLASS sop::node::GroupPromote
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// import
rttr::registration::class_<sop::node::File>("sop::file")
.constructor<>()
#define PARM_FILEPATH "sop/node/File.parm.h"
#define PARM_NODE_CLASS sop::node::File
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::ObjectMerge>("sop::object_merge")
.constructor<>()
#define PARM_FILEPATH "sop/node/ObjectMerge.parm.h"
#define PARM_NODE_CLASS sop::node::ObjectMerge
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// manipulate
rttr::registration::class_<sop::node::Delete>("sop::delete")
.constructor<>()
#define PARM_FILEPATH "sop/node/Delete.parm.h"
#define PARM_NODE_CLASS sop::node::Delete
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::Delete::NegateSelected>("sop_del_selected")
(
    REGIST_ENUM_ITEM(sop::node::Delete::NegateSelected::Delete, "dele", "Delete Selected"),
    REGIST_ENUM_ITEM(sop::node::Delete::NegateSelected::Keep,   "keep", "Delete Non-Selected")
);
rttr::registration::enumeration<sop::node::Delete::EntityType>("sop_delete_entity_type")
(
    REGIST_ENUM_ITEM(sop::node::Delete::EntityType::Points, "point", "Points"),
    REGIST_ENUM_ITEM(sop::node::Delete::EntityType::Edges,  "edge",  "Edges"),
    REGIST_ENUM_ITEM(sop::node::Delete::EntityType::Faces,  "face",  "Faces")
);
rttr::registration::enumeration<sop::node::Delete::OpRule>("sop_delete_op_rule")
(
    REGIST_ENUM_ITEM(sop::node::Delete::OpRule::Pattern,    "pattern", "Pattern"),
    REGIST_ENUM_ITEM(sop::node::Delete::OpRule::Range,      "range",   "Range"),
    REGIST_ENUM_ITEM(sop::node::Delete::OpRule::Expression, "filter",  "Expression")
);
rttr::registration::class_<sop::node::Peak>("sop::peak")
.constructor<>()
#define PARM_FILEPATH "sop/node/Peak.parm.h"
#define PARM_NODE_CLASS sop::node::Peak
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Transform>("sop::xform")
.constructor<>()
#define PARM_FILEPATH "sop/node/Transform.parm.h"
#define PARM_NODE_CLASS sop::node::Transform
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// material
rttr::registration::class_<sop::node::Color>("sop::color")
.constructor<>()
#define PARM_FILEPATH "sop/node/Color.parm.h"
#define PARM_NODE_CLASS sop::node::Color
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
REGIST_NODE_TYPE(UVLayout, uvlayout)
rttr::registration::class_<sop::node::UVQuickShade>("sop::uvquickshade")
.constructor<>()
#define PARM_FILEPATH "sop/node/UVQuickShade.parm.h"
#define PARM_NODE_CLASS sop::node::UVQuickShade
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::UVTransform>("sop::uvtransform::2.0")
.constructor<>()
#define PARM_FILEPATH "sop/node/UVTransform.parm.h"
#define PARM_NODE_CLASS sop::node::UVTransform
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::UVUnwrap>("sop::uvunwrap")
.constructor<>()
#define PARM_FILEPATH "sop/node/UVUnwrap.parm.h"
#define PARM_NODE_CLASS sop::node::UVUnwrap
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::UVUnwrap::ScaleType>("sop_uvunwrap_scale_type")
(
    REGIST_ENUM_ITEM(sop::node::UVUnwrap::ScaleType::None,    "none",    "None"),
    REGIST_ENUM_ITEM(sop::node::UVUnwrap::ScaleType::Uniform, "uniform", "Uniform"),
    REGIST_ENUM_ITEM(sop::node::UVUnwrap::ScaleType::Stretch, "stretch", "Stretch")
);

// NURBs
rttr::registration::class_<sop::node::Carve>("sop::carve")
.constructor<>()
#define PARM_FILEPATH "sop/node/Carve.parm.h"
#define PARM_NODE_CLASS sop::node::Carve
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// polygon
rttr::registration::class_<sop::node::Add>("sop::add")
.constructor<>()
.property("stdswitcher1", &sop::node::Add::GetStdSwitcher1, &sop::node::Add::SetStdSwitcher1)
.property("switcher1", &sop::node::Add::GetSwitcher1, &sop::node::Add::SetSwitcher1)
.property("points", &sop::node::Add::GetPointsNum, &sop::node::Add::SetPointsNum)
#define PARM_FILEPATH "sop/node/Add.parm.h"
#define PARM_NODE_CLASS sop::node::Add
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Boolean>("sop::boolean::2.0")
.constructor<>()
#define PARM_FILEPATH "sop/node/Boolean.parm.h"
#define PARM_NODE_CLASS sop::node::Boolean
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
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
rttr::registration::class_<sop::node::Divide>("sop::divide")
.constructor<>()
#define PARM_FILEPATH "sop/node/Divide.parm.h"
#define PARM_NODE_CLASS sop::node::Divide
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Fuse>("sop::fuse")
.constructor<>()
#define PARM_FILEPATH "sop/node/Fuse.parm.h"
#define PARM_NODE_CLASS sop::node::Fuse
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::Fuse::Operator>("sop_fuse_operator")
(
    REGIST_ENUM_ITEM(sop::node::Fuse::Operator::Consolidate,  "consolidate",   "Consolidate"),
    REGIST_ENUM_ITEM(sop::node::Fuse::Operator::UniquePoints, "unique_points", "Unique Points"),
    REGIST_ENUM_ITEM(sop::node::Fuse::Operator::Snap,         "snap",          "Snap")
);
rttr::registration::class_<sop::node::Knife>("sop::knife")
.constructor<>()
#define PARM_FILEPATH "sop/node/Knife.parm.h"
#define PARM_NODE_CLASS sop::node::Knife
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::Knife::KeepType>("sop_knife_keep")
(
    REGIST_ENUM_ITEM(sop::node::Knife::KeepType::KeepAbove, "above", "Keep Above"),
    REGIST_ENUM_ITEM(sop::node::Knife::KeepType::KeepBelow, "below", "Keep Below"),
    REGIST_ENUM_ITEM(sop::node::Knife::KeepType::KeepAll,   "both",  "Keep All")
);
rttr::registration::class_<sop::node::Normal>("sop::normal")
.constructor<>()
#define PARM_FILEPATH "sop/node/Normal.parm.h"
#define PARM_NODE_CLASS sop::node::Normal
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::Normal::AddToType>("sop_normal_add_to_type")
(
    REGIST_ENUM_ITEM(sop::node::Normal::AddToType::Points,     "typepoint",  "Points"),
    REGIST_ENUM_ITEM(sop::node::Normal::AddToType::Vertices,   "typevertex", "Vertices"),
    REGIST_ENUM_ITEM(sop::node::Normal::AddToType::Primitives, "typeprim",   "Primitives"),
    REGIST_ENUM_ITEM(sop::node::Normal::AddToType::Detail,     "typedetail", "Detail")
);
rttr::registration::class_<sop::node::PolyExtrude>("sop::polyextrude::2.0")
.constructor<>()
#define PARM_FILEPATH "sop/node/PolyExtrude.parm.h"
#define PARM_NODE_CLASS sop::node::PolyExtrude
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::PolyExtrude::Extrusion>("sop_polyextrude_extrusion")
(
    REGIST_ENUM_ITEM(sop::node::PolyExtrude::Extrusion::PrimEdgeNorm, "primnormal", "Prim Edge Norm"),
    REGIST_ENUM_ITEM(sop::node::PolyExtrude::Extrusion::PointNorm,    "pointnormal", "Point Norm")
);
rttr::registration::class_<sop::node::PolyFill>("sop::polyfill")
.constructor<>()
#define PARM_FILEPATH "sop/node/PolyFill.parm.h"
#define PARM_NODE_CLASS sop::node::PolyFill
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::PolyFill::Mode>("sop_polyfill_mode")
(
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::SinglePolygon,     "none",      "Single Polygon"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::Triangles,         "tris",      "Triangles"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::TriangleFan,       "trifan",    "Triangle Fan"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::QuadrilateralFan,  "quadfan",   "Quadrilateral Fan"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::Quadrilaterals,    "quads",     "Quadrilaterals"),
    REGIST_ENUM_ITEM(sop::node::PolyFill::Mode::QuadrilateralGrid, "gridquads", "Quadrilateral Grid")
);
rttr::registration::class_<sop::node::PolyFrame>("sop::polyframe")
.constructor<>()
#define PARM_FILEPATH "sop/node/PolyFrame.parm.h"
#define PARM_NODE_CLASS sop::node::PolyFrame
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
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
rttr::registration::class_<sop::node::Box>("sop::box")
.constructor<>()
#define PARM_FILEPATH "sop/node/Box.parm.h"
#define PARM_NODE_CLASS sop::node::Box
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
REGIST_NODE_TYPE(Curve, curve)
REGIST_NODE_TYPE(Grid, grid)
rttr::registration::class_<sop::node::Line>("sop::line")
.constructor<>()
#define PARM_FILEPATH "sop/node/Line.parm.h"
#define PARM_NODE_CLASS sop::node::Line
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Primitive>("sop::primitive")
.constructor<>()
#define PARM_FILEPATH "sop/node/Primitive.parm.h"
#define PARM_NODE_CLASS sop::node::Primitive
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
REGIST_NODE_TYPE(Sphere, sphere)
// primitive extern
rttr::registration::class_<sop::node::Dungeon>("sop::dungeon")
.constructor<>()
#define PARM_FILEPATH "sop/node/Dungeon.parm.h"
#define PARM_NODE_CLASS sop::node::Dungeon
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// rigging
REGIST_NODE_TYPE(Lattice, lattice)

// utility
rttr::registration::class_<sop::node::Blast>("sop::blast")
.constructor<>()
#define PARM_FILEPATH "sop/node/Blast.parm.h"
#define PARM_NODE_CLASS sop::node::Blast
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::CopyToPoints>("sop::copytopoints")
.constructor<>()
#define PARM_FILEPATH "sop/node/CopyToPoints.parm.h"
#define PARM_NODE_CLASS sop::node::CopyToPoints
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::ForeachPrimBegin>("sop::block_begin")
.constructor<>()
#define PARM_FILEPATH "sop/node/ForeachPrimBegin.parm.h"
#define PARM_NODE_CLASS sop::node::ForeachPrimBegin
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::enumeration<sop::node::ForeachPrimBegin::Method>("sop_foreachprimbegin_method")
(
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::Feedback,     "feedback", "Feedback"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::PieceOrPoint, "piece",    "Piece or Point"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::Metadata,     "metadata", "Metadata"),
    REGIST_ENUM_ITEM(sop::node::ForeachPrimBegin::Method::Input,        "input",    "Input")
);
rttr::registration::class_<sop::node::ForeachPrimEnd>("sop::block_end")
.constructor<>()
#define PARM_FILEPATH "sop/node/ForeachPrimEnd.parm.h"
#define PARM_NODE_CLASS sop::node::ForeachPrimEnd
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
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
rttr::registration::class_<sop::node::Output>("sop::output")
.constructor<>()
#define PARM_FILEPATH "sop/node/Output.parm.h"
#define PARM_NODE_CLASS sop::node::Output
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Python>("sop::python")
.constructor<>()
#define PARM_FILEPATH "sop/node/Python.parm.h"
#define PARM_NODE_CLASS sop::node::Python
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Split>("sop::split")
.constructor<>()
#define PARM_FILEPATH "sop/node/Split.parm.h"
#define PARM_NODE_CLASS sop::node::Split
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;
rttr::registration::class_<sop::node::Switch>("sop::switch")
.constructor<>()
#define PARM_FILEPATH "sop/node/Switch.parm.h"
#define PARM_NODE_CLASS sop::node::Switch
#include "sop/rttr_prop_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

}

namespace sop
{

void regist_rttr()
{
}

}