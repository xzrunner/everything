#include "sop/node/Grid.h"
#include "sop/GeoAdaptor.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace sop
{
namespace node
{

void Grid::Execute(Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    if (m_rows == 0 || m_columns == 0) {
        return;
    }

    BuildModel();
}

void Grid::SetSize(const sm::ivec2& size)
{
    NODE_PROP_SET(m_size, size);
}

void Grid::SetRows(size_t row)
{
    NODE_PROP_SET(m_rows, row);
}

void Grid::SetColumns(size_t col)
{
    NODE_PROP_SET(m_columns, col);
}

void Grid::BuildModel()
{
    assert(m_geo_impl);
    assert(m_rows > 0 && m_columns > 0);

    std::vector<pm3::PointPtr> points;
    points.reserve(m_columns * m_rows);
    for (size_t x = 0; x < m_columns; ++x) 
    {
        for (size_t y = 0; y < m_rows; ++y) 
        {
            auto p = std::make_shared<pm3::Point>();
            if (m_columns > 1) {
                p->pos.x = static_cast<float>(m_size.x) / (m_columns - 1) * x - 0.5f * m_size.x;
            } else {
                p->pos.x = 0;
            }
            p->pos.y = 0;
            if (m_rows > 1) {
                p->pos.z = static_cast<float>(m_size.y) / (m_rows - 1) * y - 0.5f * m_size.y;
            } else {
                p->pos.z = 0;
            }
            points.push_back(p);
        }
    }
    if (points.empty()) {
        return;
    }

    std::vector<pm3::FacePtr> faces;
    for (size_t x = 0; x < m_columns - 1; ++x)
    {
        for (size_t y = 0; y < m_rows - 1; ++y)
        {
            auto f = std::make_shared<pm3::Face>();
            f->points.push_back(x + m_columns * y);
            f->points.push_back(x + 1 + m_columns * y);
            f->points.push_back(x + 1 + m_columns * (y + 1));
            f->points.push_back(x + m_columns * (y + 1));
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

    m_geo_impl->UpdateByBrush(*brush_model);
    m_geo_impl->StoreBrush(brush_model);
}

}
}