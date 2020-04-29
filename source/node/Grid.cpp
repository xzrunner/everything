#include "sop/node/Grid.h"
#include "sop/GeoAdaptor.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

#include <polymesh3/Polytope.h>
#include <model/BrushModel.h>

namespace sop
{
namespace node
{

void Grid::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(dev, GeoAdaptor::Type::Brush);

    if (m_rows == 0 || m_columns == 0) {
        return;
    }

    BuildModel(dev);
}

void Grid::BuildModel(const ur::Device& dev)
{
    assert(m_geo_impl);
    assert(m_rows > 0 && m_columns > 0);

    std::vector<pm3::Polytope::PointPtr> points;
    points.reserve(m_columns * m_rows);
    for (int x = 0; x < m_columns; ++x)
    {
        for (int y = 0; y < m_rows; ++y)
        {
            sm::vec3 pos;
            if (m_columns > 1) {
                pos.x = static_cast<float>(m_size.x) / (m_columns - 1) * x - 0.5f * m_size.x;
            } else {
                pos.x = 0;
            }
            pos.y = 0;
            if (m_rows > 1) {
                pos.z = static_cast<float>(m_size.y) / (m_rows - 1) * y - 0.5f * m_size.y;
            } else {
                pos.z = 0;
            }
            points.push_back(std::make_shared<pm3::Polytope::Point>(pos));
        }
    }
    if (points.empty()) {
        return;
    }

    std::vector<pm3::Polytope::FacePtr> faces;
    for (int x = 0; x < m_columns - 1; ++x)
    {
        for (int y = 0; y < m_rows - 1; ++y)
        {
            auto f = std::make_shared<pm3::Polytope::Face>();
            f->border.push_back(x + m_columns * y);
            f->border.push_back(x + 1 + m_columns * y);
            f->border.push_back(x + 1 + m_columns * (y + 1));
            f->border.push_back(x + m_columns * (y + 1));
            faces.push_back(f);
        }
    }
    if (faces.empty()) {
        return;
    }

    model::BrushModel::Brush brush;
    brush.desc.mesh_begin = 0;
    brush.desc.mesh_end = 1;
    const int face_num = (m_rows - 1) * (m_columns - 1);
    brush.desc.meshes.push_back({ 0, 0, 0, face_num });
    brush.impl = std::make_shared<pm3::Polytope>(points, faces);

    auto brush_model = std::make_unique<model::BrushModel>();
    std::vector<model::BrushModel::Brush> brushes;
    brushes.push_back(brush);
    brush_model->SetBrushes(brushes);

    m_geo_impl->UpdateByBrush(dev, *brush_model);
    m_geo_impl->StoreBrush(brush_model);
}

}
}