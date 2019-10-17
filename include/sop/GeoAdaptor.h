#pragma once

#include <SM_Vector.h>
#include <halfedge/typedef.h>
#include <node0/typedef.h>

#include <functional>
#include <vector>

namespace model { class BrushModel; struct Model; }
namespace gs { class Shape3D; }

namespace sop
{

class GroupMgr;
class GeoAttribute;
class GeoShape;

class GeoAdaptor
{
public:
    enum class Type
    {
        Shape,
        Brush,
    };

public:
    GeoAdaptor(const Type& type);
    GeoAdaptor(const GeoAdaptor& adaptor);
    GeoAdaptor& operator = (const GeoAdaptor& adaptor) = delete;

    auto GetType() const { return m_type; }
    auto GetNode() const { return m_node; }

    void UpdateByBrush(GeoAttribute& attr, const model::BrushModel& brush_model);
    void StoreBrush(std::unique_ptr<model::BrushModel>& brush_model);

    void UpdateByAttr(const GeoAttribute& attr);

    const std::vector<he::PolylinePtr>& GetTopoLines() const;
    void SetTopoLines(const std::vector<he::PolylinePtr>& lines);

    // fixme: move to private
public:
    model::BrushModel* GetBrushModel() const;

private:
    void Init(const Type& type);

    void UpdateModel(const std::shared_ptr<model::Model>& model);

    static void BrushToAttr(GeoAttribute& dst, const model::BrushModel& src);
    static void AttrToBrush(model::BrushModel& dst, const GeoAttribute& src);

    static void LinesToAttr(GeoAttribute& dst, const std::vector<he::PolylinePtr>& src);
    static void AttrToLines(std::vector<he::PolylinePtr>& dst, const GeoAttribute& src);

private:
    Type m_type;

    n0::SceneNodePtr m_node = nullptr;

}; // GeoAdaptor

}