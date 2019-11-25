#ifndef PARM_INFO
#error "You must define PARM_INFO macro before include this file"
#endif

PARM_INFO(Boolean, "bool", "Boolean", 1)

PARM_INFO(Int,  "int",  "Int",  1)
PARM_INFO(Int2, "int2", "Int2", 2)
PARM_INFO(Int3, "int3", "Int3", 3)
PARM_INFO(Int4, "int4", "Int4", 4)

PARM_INFO(Float,  "float",  "Float",  1)
PARM_INFO(Float2, "float2", "Float2", 2)
PARM_INFO(Float3, "float3", "Float3", 3)
PARM_INFO(Float4, "float4", "Float4", 4)

PARM_INFO(String, "string", "String", 1)

PARM_INFO(Vector,  "vec3", "Vector",  3)
PARM_INFO(Vector4, "vec4", "Vector4", 4)

PARM_INFO(Matrix2, "mat2", "Matrix2", 4)
PARM_INFO(Matrix3, "mat3", "Matrix3", 9)
PARM_INFO(Matrix4, "mat4", "Matrix4", 16)
