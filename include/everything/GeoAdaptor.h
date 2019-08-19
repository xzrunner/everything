#pragma once

#include <SM_Vector.h>
#include <node0/typedef.h>

#include <functional>

namespace pm3 { class Polytope; }
namespace model { class BrushModel; struct Model; }
namespace gs { class Shape3D; }

namespace evt
{

class GroupMgr;
class GeoAttribute;

class GeoAdaptor
{
public:
    enum class DataType
    {
        Brush,
        Shape,
    };

public:
    GeoAdaptor(DataType type);
    GeoAdaptor(const GeoAdaptor& adaptor);
    GeoAdaptor& operator = (const GeoAdaptor& adaptor) = delete;

    auto GetNode() const { return m_node; }

    void UpdateByBrush(GeoAttribute& attr, const model::BrushModel& brush_model);
    void StoreBrush(std::unique_ptr<model::BrushModel>& brush_model);

    void UpdateByShape(GeoAttribute& attr, const std::shared_ptr<gs::Shape3D>& shape);

    void UpdateByAttr(const GeoAttribute& attr);

    // fixme: move to private
public:
    model::BrushModel* GetBrushModel() const;
    std::shared_ptr<gs::Shape3D> GetShape() const;

private:
    void UpdateModel(const std::shared_ptr<model::Model>& model);

    static void StoreToAttr(GeoAttribute& dst, const model::BrushModel& src);
    static void StoreToAttr(GeoAttribute& dst, const gs::Shape3D& src);
    static void LoadFromAttr(model::BrushModel& dst, const GeoAttribute& src);
    static void LoadFromAttr(gs::Shape3D& dst, const GeoAttribute& src);

private:
    DataType m_type;

    n0::SceneNodePtr m_node = nullptr;

}; // GeoAdaptor

}