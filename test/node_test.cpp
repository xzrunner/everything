#include "utility.h"

#include "everything/TreeContext.h"
#include "everything/Evaluator.h"

// manipulate
#include "everything/node/Transform.h"
// polygon
#include "everything/node/PolyExtrude.h"
// primitive
#include "everything/node/Box.h"
// utility
#include "everything/node/Blast.h"
#include "everything/node/CopyToPoints.h"
#include "everything/node/GroupCreate.h"
#include "everything/node/Merge.h"

#include <SM_Vector.h>
#include <polymesh3/Brush.h>
#include <model/Model.h>
#include <model/ModelExtend.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>
#include <node3/CompModelInst.h>
#include <node3/CompAABB.h>
#include <node3/CompTransform.h>

#include <catch/catch.hpp>

namespace
{

model::BrushModel* GetBrushModel(const n0::SceneNodePtr& node)
{
    REQUIRE(node);
    auto& cmodel_inst = node->GetUniqueComp<n3::CompModelInst>();
    auto& ext = cmodel_inst.GetModel()->GetModelExt();
    REQUIRE(ext);
    REQUIRE(ext->Type() == model::EXT_BRUSH);
    model::BrushModel* brush_model = static_cast<model::BrushModel*>(ext.get());
    REQUIRE(brush_model);
    return brush_model;
}

}

// manipulate

TEST_CASE("transform")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    const sm::vec3 size(1, 2, 3);
    box->SetSize(size);
    eval.AddNode(box);

    auto trans = std::make_shared<evt::node::Transform>();
    const sm::vec3 translate(4, 5, 6);
    trans->SetTranslate(translate);
    const sm::vec3 rotate(2, 3, 4);
    trans->SetRotate(rotate);
    const sm::vec3 scale(1.2f, 1.3f, 1.4f);
    trans->SetScale(scale);
    eval.AddNode(trans);

    eval.Connect({ box, 0 }, { trans, 0 });

    eval.Update();

    auto node = trans->GetSceneNode();

    // fixme
    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    auto h_sz = size * 0.5f;
    REQUIRE(aabb.Min()[0] == -h_sz.x);
    REQUIRE(aabb.Min()[1] == -h_sz.y);
    REQUIRE(aabb.Min()[2] == -h_sz.z);
    REQUIRE(aabb.Max()[0] == h_sz.x);
    REQUIRE(aabb.Max()[1] == h_sz.y);
    REQUIRE(aabb.Max()[2] == h_sz.z);

    auto& ctrans = node->GetUniqueComp<n3::CompTransform>();
    REQUIRE(ctrans.GetPosition() == translate);
    auto angle = sm::Quaternion::CreateFromEulerAngle(rotate.z, rotate.x, rotate.y);
    REQUIRE(ctrans.GetAngle() == angle);
    REQUIRE(ctrans.GetScale() == scale);
}

// primitive

TEST_CASE("box")
{
    test::init();

    auto box = std::make_shared<evt::node::Box>();

    const sm::vec3 sz(2, 3, 4);
    auto h_sz = sz * 0.5f;
    box->SetSize(sz);
    box->Execute(evt::TreeContext());
    auto node = box->GetSceneNode();
    auto brush_model = GetBrushModel(node);

    auto& brushes = brush_model->GetBrushes();
    REQUIRE(brushes.size() == 1);
    auto& brush = brushes[0];
    REQUIRE(brush.impl->faces.size() == 6);
    REQUIRE(brush.impl->vertices.size() == 8);

    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    REQUIRE(aabb.Min()[0] == -h_sz.x);
    REQUIRE(aabb.Min()[1] == -h_sz.y);
    REQUIRE(aabb.Min()[2] == -h_sz.z);
    REQUIRE(aabb.Max()[0] == h_sz.x);
    REQUIRE(aabb.Max()[1] == h_sz.y);
    REQUIRE(aabb.Max()[2] == h_sz.z);

    const sm::vec3 off(10, 11, 12);
    box->SetCenter(off);
    box->Execute(evt::TreeContext());
    REQUIRE(aabb.Min()[0] == -h_sz.x + off.x);
    REQUIRE(aabb.Min()[1] == -h_sz.y + off.y);
    REQUIRE(aabb.Min()[2] == -h_sz.z + off.z);
    REQUIRE(aabb.Max()[0] == h_sz.x + off.x);
    REQUIRE(aabb.Max()[1] == h_sz.y + off.y);
    REQUIRE(aabb.Max()[2] == h_sz.z + off.z);

    const sm::vec3 scale(5, 6, 7);
    box->SetScale(scale);
    box->Execute(evt::TreeContext());
    REQUIRE(aabb.Min()[0] == -h_sz.x * scale.x + off.x);
    REQUIRE(aabb.Min()[1] == -h_sz.y * scale.y + off.y);
    REQUIRE(aabb.Min()[2] == -h_sz.z * scale.z + off.z);
    REQUIRE(aabb.Max()[0] == h_sz.x * scale.x + off.x);
    REQUIRE(aabb.Max()[1] == h_sz.y * scale.y + off.y);
    REQUIRE(aabb.Max()[2] == h_sz.z * scale.z + off.z);
}

TEST_CASE("poly extrude")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    const sm::vec3 size(1, 2, 3);
    box->SetSize(size);
    eval.AddNode(box);

    SECTION("extrude the whole box")
    {
        auto extrude = std::make_shared<evt::node::PolyExtrude>();
        const float dist = 1.1f;
        extrude->SetDistance(dist);
        eval.AddNode(extrude);

        eval.Connect({ box, 0 }, { extrude, 0 });

        eval.Update();

        auto& caabb = extrude->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();
        auto h_sz = size * 0.5f;
        REQUIRE(aabb.Min()[0] == Approx(-h_sz.x - dist));
        REQUIRE(aabb.Min()[1] == Approx(-h_sz.y - dist));
        REQUIRE(aabb.Min()[2] == Approx(-h_sz.z - dist));
        REQUIRE(aabb.Max()[0] == Approx(h_sz.x + dist));
        REQUIRE(aabb.Max()[1] == Approx(h_sz.y + dist));
        REQUIRE(aabb.Max()[2] == Approx(h_sz.z + dist));
    }

    SECTION("extrude one face")
    {
        auto group = std::make_shared<evt::node::GroupCreate>();
        const std::string name("test");
        group->SetName(name);
        group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
        eval.AddNode(group);

        eval.Connect({ box, 0 }, { group, 0 });

        auto extrude = std::make_shared<evt::node::PolyExtrude>();
        extrude->SetGroupName(name);
        const float dist = 1.1f;
        extrude->SetDistance(dist);
        eval.AddNode(extrude);

        eval.Connect({ group, 0 }, { extrude, 0 });

        eval.Update();

        auto& caabb = extrude->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();
        auto h_sz = size * 0.5f;
        REQUIRE(aabb.Min()[0] == Approx(-h_sz.x));
        REQUIRE(aabb.Min()[1] == Approx(-h_sz.y));
        REQUIRE(aabb.Min()[2] == Approx(-h_sz.z));
        REQUIRE(aabb.Max()[0] == Approx(h_sz.x));
        REQUIRE(aabb.Max()[1] == Approx(h_sz.y + dist));
        REQUIRE(aabb.Max()[2] == Approx(h_sz.z));
    }
}

// utility

TEST_CASE("blast")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    const sm::vec3 size(1, 2, 3);
    box->SetSize(size);
    eval.AddNode(box);

    auto group = std::make_shared<evt::node::GroupCreate>();
    const std::string name("test");
    group->SetName(name);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<evt::node::Blast>();
    blast->SetGroupName(name);
    blast->SetGroupType(evt::node::GroupType::Primitives);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    SECTION("del selected")
    {
        eval.Update();

        auto brush_model = GetBrushModel(blast->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0].impl;
        REQUIRE(brush->faces.size() == 5);
    }

    SECTION("del non selected")
    {
        blast->SetDeleteNonSelected(true);

        eval.Update();

        auto brush_model = GetBrushModel(blast->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0].impl;
        REQUIRE(brush->faces.size() == 1);
    }
}

TEST_CASE("copy to points")
{
    test::init();

    evt::Evaluator eval;

    auto src_box = std::make_shared<evt::node::Box>();
    const sm::vec3 src_size(1, 2, 3);
    src_box->SetSize(src_size);
    const sm::vec3 src_pos(21, 22, 23);
    src_box->SetCenter(src_pos);
    eval.AddNode(src_box);

    auto target_box = std::make_shared<evt::node::Box>();
    const sm::vec3 target_pos(5, 6, 7);
    const sm::vec3 target_size(1.1f, 2.2f, 3.3f);
    target_box->SetCenter(target_pos);
    target_box->SetSize(target_size);
    eval.AddNode(target_box);

    auto copy = std::make_shared<evt::node::CopyToPoints>();
    eval.AddNode(copy);

    eval.Connect({ src_box, 0 },    { copy, evt::node::CopyToPoints::IDX_SRC_PRIM });
    eval.Connect({ target_box, 0 }, { copy, evt::node::CopyToPoints::IDX_TARGET_POS });

    eval.Update();

    auto& caabb = copy->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    auto h_src_sz = src_size * 0.5f;
    auto h_tar_sz = target_size * 0.5f;
    REQUIRE(aabb.Min()[0] == Approx(src_pos.x + target_pos.x - h_src_sz.x - h_tar_sz.x));
    REQUIRE(aabb.Min()[1] == Approx(src_pos.y + target_pos.y - h_src_sz.y - h_tar_sz.y));
    REQUIRE(aabb.Min()[2] == Approx(src_pos.z + target_pos.z - h_src_sz.z - h_tar_sz.z));
    REQUIRE(aabb.Max()[0] == Approx(src_pos.x + target_pos.x + h_src_sz.x + h_tar_sz.x));
    REQUIRE(aabb.Max()[1] == Approx(src_pos.y + target_pos.y + h_src_sz.y + h_tar_sz.y));
    REQUIRE(aabb.Max()[2] == Approx(src_pos.z + target_pos.z + h_src_sz.z + h_tar_sz.z));
}

TEST_CASE("merge")
{
    test::init();

    evt::Evaluator eval;

    auto box0 = std::make_shared<evt::node::Box>();
    const sm::vec3 size0(1, 2, 3);
    box0->SetSize(size0);
    const sm::vec3 pos0(21, 22, 23);
    box0->SetCenter(pos0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<evt::node::Box>();
    const sm::vec3 size1(11, 12, 13);
    box1->SetSize(size1);
    const sm::vec3 pos1(121, 122, 123);
    box1->SetCenter(pos1);
    eval.AddNode(box1);

    auto box2 = std::make_shared<evt::node::Box>();
    const sm::vec3 size2(21, 22, 23);
    box2->SetSize(size2);
    const sm::vec3 pos2(221, 222, 223);
    box2->SetCenter(pos2);
    eval.AddNode(box2);

    auto merge = std::make_shared<evt::node::Merge>();
    eval.AddNode(merge);

    eval.Connect({ box0, 0 }, { merge, 0 });
    eval.Connect({ box1, 0 }, { merge, 1 });
    eval.Connect({ box2, 0 }, { merge, 2 });

    eval.Update();

    auto brush_model = GetBrushModel(merge->GetSceneNode());
    auto& brushes = brush_model->GetBrushes();
    REQUIRE(brushes.size() == 3);

    auto& caabb = merge->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    REQUIRE(aabb.Min()[0] == Approx(std::min(pos0.x - size0.x * 0.5f, std::min(pos1.x - size1.x * 0.5f, pos2.x - size2.x * 0.5f))));
    REQUIRE(aabb.Min()[1] == Approx(std::min(pos0.y - size0.y * 0.5f, std::min(pos1.y - size1.y * 0.5f, pos2.y - size2.y * 0.5f))));
    REQUIRE(aabb.Min()[2] == Approx(std::min(pos0.z - size0.z * 0.5f, std::min(pos1.z - size1.z * 0.5f, pos2.z - size2.z * 0.5f))));
    REQUIRE(aabb.Max()[0] == Approx(std::max(pos0.x + size0.x * 0.5f, std::max(pos1.x + size1.x * 0.5f, pos2.x + size2.x * 0.5f))));
    REQUIRE(aabb.Max()[1] == Approx(std::max(pos0.y + size0.y * 0.5f, std::max(pos1.y + size1.y * 0.5f, pos2.y + size2.y * 0.5f))));
    REQUIRE(aabb.Max()[2] == Approx(std::max(pos0.z + size0.z * 0.5f, std::max(pos1.z + size1.z * 0.5f, pos2.z + size2.z * 0.5f))));
}