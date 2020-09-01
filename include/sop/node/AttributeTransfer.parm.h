#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(EnablePointAttribs,     bool, pointattribs,     m_point_attribs,     (true))
PARAM_INFO(EnableVertexAttribs,    bool, vertexattribs,    m_vertex_attribs,    (false))
PARAM_INFO(EnablePrimitiveAttribs, bool, primitiveattribs, m_primitive_attribs, (true))
PARAM_INFO(EnableDetailAttribs,    bool, detailattribs,    m_detail_attribs,    (false))

PARAM_INFO(PointAttribList,     std::vector<std::string>, pointattriblist,  m_point_attrib_list,  ())
PARAM_INFO(VertexAttribList,    std::vector<std::string>, vertexattriblist, m_vertex_attrib_list, ())
PARAM_INFO(PrimitiveAttribList, std::vector<std::string>, primattriblist,   m_prim_attrib_list,   ())
PARAM_INFO(DetailAttribList,    std::vector<std::string>, detailattriblist, m_detail_attrib_list, ())
