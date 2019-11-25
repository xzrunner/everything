#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(EnablePointAttribs,     Bool, pointattribs,     m_point_attribs,     (true))
PARAM_INFO(EnableVertexAttribs,    Bool, vertexattribs,    m_vertex_attribs,    (false))
PARAM_INFO(EnablePrimitiveAttribs, Bool, primitiveattribs, m_primitive_attribs, (true))
PARAM_INFO(EnableDetailAttribs,    Bool, detailattribs,    m_detail_attribs,    (false))

PARAM_INFO(PointAttribList,     Array<String>, pointattriblist,  m_point_attrib_list,  ())
PARAM_INFO(VertexAttribList,    Array<String>, vertexattriblist, m_vertex_attrib_list, ())
PARAM_INFO(PrimitiveAttribList, Array<String>, primattriblist,   m_prim_attrib_list,   ())
PARAM_INFO(DetailAttribList,    Array<String>, detailattriblist, m_detail_attrib_list, ())
