#ifndef GEO_ATTR_INFO
#error "You must define GEO_ATTR_INFO macro before include this file"
#endif

//
// Geometry attributes
//

// Point position. The viewport uses this to lay out the points of the model in 3D space. You can overwrite this attribute to move the point.
GEO_ATTR_INFO(GEO_ATTR_POS,    "P",    Vector)
// Normal direction. You can overwrite this attribute to change the normal.
GEO_ATTR_INFO(GEO_ATTR_NORM,   "N",    Vector)
// Velocity. The renderer uses this attribute to know where to add motion blur. This attribute is not computed automatically, but several nodes, especially particle DOPs, can set/use it. You can add velocity to points using the Trail SOP.
GEO_ATTR_INFO(GEO_ATTR_SPD,    "v",    Vector)
// A unique element ID. This is not the same as the element number (for example, the point number). This is an attribute you can, for example, assign to points to keep track of them even if the point numbers change (which can happen if the number of points changes). Particle DOPs often set/use this attribute.
GEO_ATTR_INFO(GEO_ATTR_ID,    "id",    Int)
// This is a value you can set on primitives, such as volumes or packed primitives, to be able to find them in code by name. Some nodes set/read this attribute.
GEO_ATTR_INFO(GEO_ATTR_NAME,  "name",  String)
// Nodes the break up geometry into pieces will often set this attribute so you can tell which polygonal faces are part of the same piece. Faces in the same piece will share the same value in their piece attribute. Other nodes may use this attribute to operate on pieces.
GEO_ATTR_INFO(GEO_ATTR_PIECE, "piece", Int)

//
// Sizes and rotations
//

// Uniform scaling factor. This is used in different ways in different places. For particle/point rendering, it controls the size of the particle/point (in world space units) at render time. For instancing, you can use it to uniformly scale the instanced geometry.
GEO_ATTR_INFO(GEO_ATTR_PSCALE, "pscale", Float)
// Whereas pscale is a single uniform scaling factor in all axes, this attribute lets you scale non-uniformly along each axis separately. See the pscale attribute for more information.
GEO_ATTR_INFO(GEO_ATTR_SCALE,  "scale",  Vector)

//
// Particle attributes
//

// Represents the up vector of a particle¡¯s local space. This is used for instancing geometry onto a point. You can overwrite this attribute to change the particle¡¯s orientation.
GEO_ATTR_INFO(GEO_ATTR_UP,     "up",     Vector)
// Quaternion orientation of a particle. Allows fully specifying rotation, whereas up only orients along a single axis. If this exists, it is used for instancing geometry onto a point.
GEO_ATTR_INFO(GEO_ATTR_ORIENT, "orient", Vector4)
// An additional offset-quaternion applied after all other attributes, used when instancing geometry onto a point.
GEO_ATTR_INFO(GEO_ATTR_ROT,    "rot",    Vector4)

//
// Shader overrides
//

// Diffuse color override. The viewport uses this to color OpenGL geometry.
GEO_ATTR_INFO(GEO_ATTR_CD, "Cd", Vector)

//
// rendering attributes
//

// The UV coordinates of the point/vertex. The first two elements of this vector contain the current U and V coordinates. Note that uv is Houdini¡¯s conventional attribute name for storing texture coordinates. You can store texture coordinates in other attributes, so you can have multiple UV layouts on the same geometry. Most texture-related nodes have a field to let you specify the name of the UV attribute to use.
GEO_ATTR_INFO(GEO_ATTR_UV,       "uv",       Float3)
// The node path to the material to use to shade this primitive. Overrides the object¡¯s material.
GEO_ATTR_INFO(GEO_ATTR_MATERIAL, "material", String)

//
// Copying and instancing point attributes
//

// Local pivot point for the copy
GEO_ATTR_INFO(GEO_ATTR_PIVOT, "pivot", Vector)
// Translation of the copy, in addition to P
GEO_ATTR_INFO(GEO_ATTR_TRANS, "trans", Vector)
