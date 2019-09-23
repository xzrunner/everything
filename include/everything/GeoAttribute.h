#pragma once

#include "everything/GeoAttrType.h"
#include "everything/Variable.h"
#include "everything/VarValue.h"

#include <SM_Cube.h>

#include <vector>
#include <memory>

#include <boost/noncopyable.hpp>

namespace evt
{

class GeoShape;

class GeoAttribute : boost::noncopyable
{
public:
    struct Point
    {
        Point() {}
        Point(const sm::vec3& pos) : pos(pos) {}
        Point(const Point& point) : pos(point.pos), vars(point.vars) {}
        Point& operator = (const Point& point) {
            pos = point.pos;
            vars = point.vars;
            return *this;
        }

        sm::vec3 pos;

        std::vector<VarValue> vars;

    }; // Point

    struct Primitive;
    struct Vertex
    {
        Vertex() {}
        Vertex(const Vertex& vert) : vars(vert.vars) {}
        Vertex& operator = (const Vertex& vert) {
            vars = vert.vars;
            return *this;
        }

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
        Primitive(Type type) : type(type) {}
        Primitive(const Primitive& prim) : type(prim.type), vars(prim.vars) {}
        Primitive& operator = (const Primitive& prim) {
            type = prim.type;
            vars = prim.vars;
            return *this;
        }

        sm::vec3 CalcNormal() const;

        std::vector<std::shared_ptr<Vertex>> vertices;

        Type type = Type::PolygonFace;

        std::vector<VarValue> vars;

    }; // Primitive

    struct Detail
    {
        std::vector<VarValue> vars;

    }; // Detail

    struct VarDesc
    {
        std::string name;
        VarType     type;
    };

public:
    GeoAttribute() {}
    GeoAttribute(const std::vector<std::shared_ptr<GeoShape>>& shapes);
    GeoAttribute(const GeoAttribute& attr);
    GeoAttribute& operator = (const GeoAttribute& attr);

    auto& GetPoints() const    { return m_points; }
    auto& GetVertices() const  { return m_vertices; }
    auto& GetPrimtives() const { return m_primtives; }
    auto& GetDetail() const    { return m_detail; }

    void RemoveItems(GeoAttrType type, const std::vector<bool>& del_flags, bool del_unused_pt);

    void ChangePointsOrder(const std::vector<size_t>& order);

    void AddAttr(GeoAttrType type, const VarDesc& var_desc, const std::vector<VarValue>& var_list);
    Variable QueryAttr(GeoAttrType type, const std::string& name, size_t index) const;

    void Combine(const GeoAttribute& attr);

    void FromGeoShapes(const std::vector<std::shared_ptr<GeoShape>>& shapes);

    auto& GetAABB() const { return m_aabb; }

    auto& GetAttrDesc(GeoAttrType type) const {
        return m_var_descs[static_cast<int>(type)];
    }
    void SetAttrDesc(GeoAttrType type, const std::vector<VarDesc>& desc) {
        m_var_descs[static_cast<int>(type)] = desc;
    }

    std::vector<VarValue> GetDefaultValues(GeoAttrType type) const;

    template<typename T>
    int QueryIndex(const T& i) const;

private:
    void Clear();

    void SetupAABB();

    void CombineAttrDesc(const GeoAttribute& attr, GeoAttrType type,
        std::vector<uint32_t>& indices, std::vector<VarValue>& default_vars);

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

    std::vector<VarDesc> m_var_descs[static_cast<int>(GeoAttrType::MaxTypeNum)];

    sm::cube m_aabb;

    friend class GeoAdaptor;

}; // GeoAttribute

}

#include "everything/GeoAttribute.inl"