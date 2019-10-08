#pragma once

#include <halfedge/Polyline.h>
#include <node0/NodeComp.h>
#include <SM_Vector.h>

namespace sop
{

class CompTopoPolyline : public n0::NodeComp
{
public:
    CompTopoPolyline();

    virtual const char* Type() const override { return TYPE_NAME; }
    virtual n0::CompID TypeID() const override {
        return n0::GetCompTypeID<CompTopoPolyline>();
    }
    virtual std::unique_ptr<n0::NodeComp> Clone(const n0::SceneNode& node) const override;

    auto GetLines() const { return m_lines; }
    void SetLines(const std::vector<he::PolylinePtr>& lines) { m_lines = lines; }

    static const char* const TYPE_NAME;

private:
    std::vector<he::PolylinePtr> m_lines;

}; // CompTopoPolyline

}