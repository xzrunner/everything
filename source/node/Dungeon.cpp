#include "sop/node/Dungeon.h"
#include "sop/GeoAdaptor.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

#include <dungeon/BaseBSPGen.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace sop
{
namespace node
{

void Dungeon::Execute(Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    BuildModel();
}

void Dungeon::BuildModel()
{
    assert(m_geo_impl);

    dungeon::BaseBSPGen dungeon(m_size, m_split_sz, m_min_sz, m_seed);
    auto rooms = dungeon.GetAllRooms();
    auto passages = dungeon.GetAllPassages();
    std::copy(passages.begin(), passages.end(), std::back_inserter(rooms));
    if (rooms.empty()) {
        return;
    }

    std::vector<pm3::PointPtr> points;
    std::vector<pm3::FacePtr> faces;
    for (auto& r : rooms)
    {
        auto f = std::make_shared<pm3::Face>();
        for (size_t i = 0; i < 4; ++i) {
            f->points.push_back(points.size() + i);
        }
        faces.push_back(f);

        points.push_back(std::make_shared<pm3::Point>(sm::vec3(r.xmin, 0, r.ymin)));
        points.push_back(std::make_shared<pm3::Point>(sm::vec3(r.xmin, 0, r.ymax)));
        points.push_back(std::make_shared<pm3::Point>(sm::vec3(r.xmax, 0, r.ymax)));
        points.push_back(std::make_shared<pm3::Point>(sm::vec3(r.xmax, 0, r.ymin)));
    }

    model::BrushModel::Brush brush;
    brush.desc.mesh_begin = 0;
    brush.desc.mesh_end = 1;
    brush.desc.meshes.push_back({ 0, 0, 0, static_cast<int>(rooms.size()) });
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