#include "sop/node/File.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAdaptor.h"
#include "sop/ParmList.h"

#include <model/AssimpHelper.h>
#include <model/BrushModel.h>
#include <polymesh3/Polytope.h>

namespace sop
{
namespace node
{

void File::Execute(Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    if (m_filepath.empty()) {
        return;
    }

    std::vector<std::unique_ptr<model::MeshRawData>> meshes;
    if (!model::AssimpHelper::Load(meshes, m_filepath)) {
        return;
    }

    std::vector<sm::vec3> normals;
    std::vector<sm::vec2> texcoords;

    auto brush_model = std::make_unique<model::BrushModel>();
    std::vector<model::BrushModel::Brush> brushes;
    for (auto& src : meshes)
    {
        std::copy(src->normals.begin(), src->normals.end(), std::back_inserter(normals));
        std::copy(src->texcoords.begin(), src->texcoords.end(), std::back_inserter(texcoords));

        std::vector<pm3::Polytope::PointPtr> points;
        points.reserve(src->vertices.size());
        for (auto& s : src->vertices) {
            points.push_back(std::make_shared<pm3::Polytope::Point>(s));
        }

        std::vector<pm3::Polytope::FacePtr> faces;
        for (auto& s : src->faces)
        {
            auto f = std::make_shared<pm3::Polytope::Face>();
            f->border.resize(s.size());
            for (size_t i = 0, n = s.size(); i < n; ++i) {
                f->border[i] = s[i];
            }
            faces.push_back(f);
        }

        model::BrushModel::Brush brush;

        brush.desc.mesh_begin = 0;
        brush.desc.mesh_end = 1;
        const int face_num = src->faces.size();
        brush.desc.meshes.push_back({ 0, 0, 0, face_num });

        brush.impl = std::make_shared<pm3::Polytope>(points, faces);

        brushes.push_back(brush);
    }
    brush_model->SetBrushes(brushes);

    m_geo_impl->UpdateByBrush(*brush_model);
    m_geo_impl->StoreBrush(brush_model);

    // set norm and uv attr
    auto& attr = m_geo_impl->GetAttr();
    if (!normals.empty())
    {
        assert(normals.size() == attr.GetPoints().size());
        attr.AddParmList(GeoAttrClass::Point,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, normals)
        );
    }
    if (!texcoords.empty())
    {
        assert(texcoords.size() == attr.GetPoints().size());
        std::vector<sm::vec3> data;
        data.reserve(texcoords.size());
        for (auto& tc : texcoords) {
            data.push_back(sm::vec3(tc.x, tc.y, 0));
        }
        attr.AddParmList(GeoAttrClass::Point,
            std::make_shared<ParmFlt3List>(GEO_ATTR_UV, data)
        );
    }
}

}
}