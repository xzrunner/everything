#pragma once

#include "sop/GeoAttrClass.h"
#include "sop/Variable.h"
#include "sop/GeoAttrDefine.h"

#include <SM_Cube.h>
#include <halfedge/typedef.h>
#include <halfedge/TopoID.h>

#include <vector>
#include <memory>

#include <boost/noncopyable.hpp>

namespace sop
{

class GeoShape;
class ParmList;

class GeoAttribute : boost::noncopyable
{
public:
    struct Point
    {
        Point() {}
        Point(const sm::vec3& pos);

        sm::vec3 pos;

        size_t attr_idx = 0;

        he::TopoID topo_id;

        size_t prim_id = 0;

    }; // Point

    struct Primitive;
    struct Vertex
    {
        Vertex() {}
        Vertex(const Vertex& vert);
        Vertex& operator = (const Vertex& vert);

        std::shared_ptr<Point>   point = nullptr;
        std::weak_ptr<Primitive> prim;

        size_t attr_idx = 0;

    }; // Vertex

    // In Houdini, primitives refer to a unit of geometry, lower-level than an object but above points.
    struct Primitive
    {
        enum class Type
        {
            PolygonFace = 0,
            PolygonCurves,
        };

        Primitive() {}
        Primitive(Type type);
        Primitive(const Primitive& prim);
        Primitive& operator = (const Primitive& prim);

        sm::vec3 CalcNormal() const;

        std::vector<std::shared_ptr<Vertex>> vertices;

        Type type = Type::PolygonFace;

        size_t attr_idx = 0;

        he::TopoID topo_id;

        size_t prim_id = 0;

    }; // Primitive

    struct Detail
    {

    }; // Detail

public:
    GeoAttribute() {}
    GeoAttribute(const GeoAttribute& attr);
    GeoAttribute& operator = (const GeoAttribute& attr);

    auto& GetPoints() const    { return m_points; }
    auto& GetVertices() const  { return m_vertices; }
    auto& GetPrimtives() const { return m_primtives; }
    auto& GetDetail() const    { return m_detail; }

    size_t GetSize(GeoAttrClass cls) const;

    void RemoveItems(GeoAttrClass cls, const std::vector<bool>& del_flags, bool del_unused_pt);

    void ChangePointsOrder(const std::vector<size_t>& order);

    void AddParmList(GeoAttrClass cls, const std::shared_ptr<ParmList>& list);
    bool RemoveParmList(GeoAttrClass cls, const std::string& name);
    std::shared_ptr<ParmList> QueryParmList(GeoAttrClass cls, const std::string& name) const;
    std::shared_ptr<ParmList> QueryParmList(GeoAttrClass cls, GeoAttr attr) const;

    auto& GetAllParmLists() const { return m_parm_lists; }
    void SetParmLists(GeoAttrClass cls, const std::vector<std::shared_ptr<ParmList>>& lists);

    Variable QueryParm(GeoAttrClass cls, const std::string& name, size_t index) const;

    void Combine(const GeoAttribute& attr);

    void SetTopoLines(const std::vector<he::PolylinePtr>& lines);

    auto& GetAABB() const { return m_aabb; }

private:
    void Clear();

    void SetupAttrIndices();
    void SetupAABB();

    void CombineParmLists(const GeoAttribute& attr, GeoAttrClass cls);

    void CombineTopoID(const GeoAttribute& attr);
    void CombineBrushID(const GeoAttribute& attr);

    void CombinePoints(const GeoAttribute& attr);
    void CombineVertices(const GeoAttribute& attr);
    void CombinePrimitives(const GeoAttribute& attr);
    void CombineDetail(const GeoAttribute& attr);

    void UpdatePointsChanged(bool del_unused_pt);
    void UpdateVerticesChanged(bool del_unused_pt);
    void UpdatePrimitivesChanged(bool del_unused_pt);

private:
    std::vector<std::shared_ptr<Point>>     m_points;
    std::vector<std::shared_ptr<Vertex>>    m_vertices;
    std::vector<std::shared_ptr<Primitive>> m_primtives;
    Detail                                  m_detail;

    static const size_t MAX_LSIT_COUNT = static_cast<size_t>(GeoAttrClass::MaxTypeNum);
    std::vector<std::shared_ptr<ParmList>> m_parm_lists[MAX_LSIT_COUNT];

    sm::cube m_aabb;

    friend class GeoAdaptor;

}; // GeoAttribute

}