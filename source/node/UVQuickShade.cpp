#include "sop/node/UVQuickShade.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

#include <model/AssimpHelper.h>
#include <model/TextureLoader.h>
#include <model/Model.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>

namespace sop
{
namespace node
{

void UVQuickShade::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    auto node = m_geo_impl->GetNode();
    if (!node) {
        return;
    }

    if (!node->HasSharedComp<n3::CompModel>()) {
        return;
    }

    auto& cmodel = node->GetSharedComp<n3::CompModel>();
    if (!cmodel.GetModel()) {
        return;
    }

    auto tex = model::TextureLoader::LoadFromFile(dev, m_image_file.c_str());
    if (!tex) {
        return;
    }

    m_geo_impl->UpdateByAttr(dev);

    auto& model = cmodel.GetModel();

    m_diffuse_tex = model->textures.size();
    cmodel.GetModel()->textures.push_back({ m_image_file, tex });

    assert(model->materials.size() == 1);
    model->materials[0]->diffuse_tex = m_diffuse_tex;
}

}
}