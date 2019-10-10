#pragma once

#include "sop/GeoAttrClass.h"
#include "sop/Variable.h"
#include "sop/VarValue.h"
#include "sop/GeoAttrType.h"
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

class GeoAttribute : boost::noncopyable
{
public:
    struct Point
    {
        Point() {}
        Point(const sm::vec3& pos);

        sm::vec3 pos;

        std::vector<VarValue> vars;

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

        std::vector<VarValue> vars;

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

        std::vector<VarValue> vars;

        he::TopoID topo_id;

        size_t prim_id = 0;

    }; // Primitive

    struct Detail
    {
        std::vector<VarValue> vars;

    }; // Detail

    class VarDesc
    {
    public:
        VarDesc(GeoAttr attr);
        VarDesc(const std::string& name, GeoAttrType type);

        auto& GetAttr() const { return m_attr; }

        auto& GetName() const { return m_name; }
        auto& GetType() const { return m_type; }

        void SetType(GeoAttrType type) { m_type = type; }

    private:
        GeoAttr     m_attr = GEO_ATTR_UNKNOWN;

        std::string m_name;
        GeoAttrType m_type;

    }; // VarDesc

public:
    GeoAttribute() {}
    GeoAttribute(const GeoAttribute& attr);
    GeoAttribute& operator = (const GeoAttribute& attr);

    auto& GetPoints() const    { return m_points; }
    auto& GetVertices() const  { return m_vertices; }
    auto& GetPrimtives() const { return m_primtives; }
    auto& GetDetail() const    { return m_detail; }

    void RemoveItems(GeoAttrClass cls, const std::vector<bool>& del_flags, bool del_unused_pt);

    void ChangePointsOrder(const std::vector<size_t>& order);

    void AddAttr(GeoAttrClass cls, const VarDesc& var_desc, const std::vector<VarValue>& var_list);
    Variable QueryAttr(GeoAttrClass cls, const std::string& name, size_t index) const;

    void Combine(const GeoAttribute& attr);

    void SetTopoLines(const std::vector<he::PolylinePtr>& lines);

    auto& GetAABB() const { return m_aabb; }

    auto& GetAttrDesc(GeoAttrClass cls) const {
        return m_var_descs[static_cast<int>(cls)];
    }
    void SetAttrDesc(GeoAttrClass cls, const std::vector<VarDesc>& desc) {
        m_var_descs[static_cast<int>(cls)] = desc;
    }

    std::vector<VarValue> GetDefaultValues(GeoAttrClass cls) const;

    int QueryAttrIdx(GeoAttrClass cls, GeoAttr attr) const;

    template<typename T>
    int QueryIndex(const T& i) const;

private:
    void Clear();

    void SetupAABB();

    void CombineAttrDesc(const GeoAttribute& attr, GeoAttrClass cls,
        std::vector<uint32_t>& indices, std::vector<VarValue>& default_vars);

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

    std::vector<VarDesc> m_var_descs[static_cast<int>(GeoAttrClass::MaxTypeNum)];

    sm::cube m_aabb;

    friend class GeoAdaptor;

}; // GeoAttribute

}

#include "sop/GeoAttribute.inl"
