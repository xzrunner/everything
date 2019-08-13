#include "utility.h"

#include "everything/TreeContext.h"
#include "everything/Evaluator.h"

// manipulate
#include "everything/node/Transform.h"
// polygon
#include "everything/node/Boolean.h"
#include "everything/node/Knife.h"
#include "everything/node/PolyExtrude.h"
// primitive
#include "everything/node/Box.h"
// utility
#include "everything/node/Blast.h"
#include "everything/node/CopyToPoints.h"
#include "everything/node/GroupCreate.h"
#include "everything/node/Merge.h"

#include <SM_Vector.h>
#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
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

void CheckAABB(const pt3::AABB& aabb, const sm::vec3& min, const sm::vec3& max)
{
    REQUIRE(aabb.Min()[0] == Approx(min.x));
    REQUIRE(aabb.Min()[1] == Approx(min.y));
    REQUIRE(aabb.Min()[2] == Approx(min.z));
    REQUIRE(aabb.Max()[0] == Approx(max.x));
    REQUIRE(aabb.Max()[1] == Approx(max.y));
    REQUIRE(aabb.Max()[2] == Approx(max.z));
}

}

// manipulate

TEST_CASE("transform")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    const sm::vec3 size(1, 2, 3);
    auto h_sz = size * 0.5f;
    box->SetSize(size);
    eval.AddNode(box);

    auto trans = std::make_shared<evt::node::Transform>();
    eval.AddNode(trans);

    eval.Connect({ box, 0 }, { trans, 0 });

    SECTION("scale")
    {
        const sm::vec3 scale(2, 3, 4);
        trans->SetScale(scale);

        eval.Update();

        auto& caabb = trans->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();

        CheckAABB(aabb, -h_sz * scale, h_sz * scale);
    }

    SECTION("rotate x")
    {
        const sm::vec3 rot(90, 0, 0);
        trans->SetRotate(rot);

        eval.Update();

        auto& caabb = trans->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();

        CheckAABB(aabb, sm::vec3(-h_sz.x, -h_sz.z, -h_sz.y), sm::vec3(h_sz.x, h_sz.z, h_sz.y));
    }

    SECTION("rotate y")
    {
        const sm::vec3 rot(0, 90, 0);
        trans->SetRotate(rot);

        eval.Update();

        auto& caabb = trans->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();

        CheckAABB(aabb, sm::vec3(-h_sz.z, -h_sz.y, -h_sz.x), sm::vec3(h_sz.z, h_sz.y, h_sz.x));
    }

    SECTION("rotate z")
    {
        const sm::vec3 rot(0, 0, 90);
        trans->SetRotate(rot);

        eval.Update();

        auto& caabb = trans->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();

        CheckAABB(aabb, sm::vec3(-h_sz.y, -h_sz.x, -h_sz.z), sm::vec3(h_sz.y, h_sz.x, h_sz.z));
    }

    SECTION("translate")
    {
        const sm::vec3 off(1.1f, 2.2f, 3.3f);
        trans->SetTranslate(off);

        eval.Update();

        auto& caabb = trans->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();

        CheckAABB(aabb, -h_sz + off, h_sz + off);
    }
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
    REQUIRE(brush.impl->Faces().size() == 6);
    REQUIRE(brush.impl->Points().size() == 8);

    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    CheckAABB(aabb, -h_sz, h_sz);

    const sm::vec3 off(10, 11, 12);
    box->SetCenter(off);
    box->Execute(evt::TreeContext());
    CheckAABB(aabb, -h_sz + off, h_sz + off);

    const sm::vec3 scale(5, 6, 7);
    box->SetScale(scale);
    box->Execute(evt::TreeContext());
    CheckAABB(aabb, -h_sz * scale + off, h_sz * scale + off);
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
        CheckAABB(aabb, -h_sz - sm::vec3(dist, dist, dist), h_sz + sm::vec3(dist, dist, dist));
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
        CheckAABB(aabb, -h_sz, sm::vec3(h_sz.x, h_sz.y + dist, h_sz.z));
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
    blast->SetGroupType(pm3::GroupType::Face);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    SECTION("del selected")
    {
        eval.Update();

        auto brush_model = GetBrushModel(blast->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0].impl;
        REQUIRE(brush->Faces().size() == 5);
    }

    SECTION("del non selected")
    {
        blast->SetDeleteNonSelected(true);

        eval.Update();

        auto brush_model = GetBrushModel(blast->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0].impl;
        REQUIRE(brush->Faces().size() == 1);
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
    CheckAABB(aabb, src_pos + target_pos - h_src_sz - h_tar_sz, src_pos + target_pos + h_src_sz + h_tar_sz);
}

TEST_CASE("group_create")
{
    test::init();

    evt::Evaluator eval;

    auto box0 = std::make_shared<evt::node::Box>();
    const sm::vec3 pos0(5, 0, 0);
    box0->SetCenter(pos0);
    eval.AddNode(box0);

    auto group = std::make_shared<evt::node::GroupCreate>();
    group->SetName("test");
    group->EnableKeepByNormals(sm::vec3(0, 0, 1), 10);
    eval.AddNode(group);

    eval.Connect({ box0, 0 }, { group, 0 });

    auto box1 = std::make_shared<evt::node::Box>();
    const sm::vec3 pos1(-5, 0, 0);
    box1->SetCenter(pos1);
    eval.AddNode(box1);

    auto merge = std::make_shared<evt::node::Merge>();
    eval.AddNode(merge);
    eval.Connect({ group, 0 }, { merge, 0 });
    eval.Connect({ box1, 0 }, { merge, 1 });

    auto extrude = std::make_shared<evt::node::PolyExtrude>();
    extrude->SetGroupName("test");
    extrude->SetDistance(5);
    eval.AddNode(extrude);
    eval.Connect({ merge, 0 }, { extrude, 0 });

    eval.Update();

    auto brush_model = GetBrushModel(merge->GetSceneNode());
    auto& brushes = brush_model->GetBrushes();
    REQUIRE(brushes.size() == 1);
    auto& brush = brushes[0];
    REQUIRE(brush.impl->Faces().size() == 12);
    REQUIRE(brush.impl->Points().size() == 16);

    auto& caabb = extrude->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    CheckAABB(aabb, sm::vec3(-5.5f, -0.5f, -0.5f), sm::vec3(5.5f, 0.5f, 5.5f));
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
    REQUIRE(brushes.size() == 1);
    auto& brush = brushes[0];
    REQUIRE(brush.impl->Faces().size() == 18);
    REQUIRE(brush.impl->Points().size() == 24);

    auto& caabb = merge->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    REQUIRE(aabb.Min()[0] == Approx(std::min(pos0.x - size0.x * 0.5f, std::min(pos1.x - size1.x * 0.5f, pos2.x - size2.x * 0.5f))));
    REQUIRE(aabb.Min()[1] == Approx(std::min(pos0.y - size0.y * 0.5f, std::min(pos1.y - size1.y * 0.5f, pos2.y - size2.y * 0.5f))));
    REQUIRE(aabb.Min()[2] == Approx(std::min(pos0.z - size0.z * 0.5f, std::min(pos1.z - size1.z * 0.5f, pos2.z - size2.z * 0.5f))));
    REQUIRE(aabb.Max()[0] == Approx(std::max(pos0.x + size0.x * 0.5f, std::max(pos1.x + size1.x * 0.5f, pos2.x + size2.x * 0.5f))));
    REQUIRE(aabb.Max()[1] == Approx(std::max(pos0.y + size0.y * 0.5f, std::max(pos1.y + size1.y * 0.5f, pos2.y + size2.y * 0.5f))));
    REQUIRE(aabb.Max()[2] == Approx(std::max(pos0.z + size0.z * 0.5f, std::max(pos1.z + size1.z * 0.5f, pos2.z + size2.z * 0.5f))));
}

TEST_CASE("knife")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto knife = std::make_shared<evt::node::Knife>();
    eval.AddNode(knife);

    eval.Connect({ box, 0 }, { knife, 0 });

    SECTION("keep all")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepAll);

        eval.Update();

        auto brush_model = GetBrushModel(knife->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0];
        REQUIRE(brush.impl->Points().size() == 12);
        REQUIRE(brush.impl->Faces().size() == 10);
        REQUIRE(brush.impl->GetHalfedge()->GetEdges().Size() == 40);

        auto& caabb = knife->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();
        CheckAABB(aabb, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep above")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepAbove);

        eval.Update();

        auto brush_model = GetBrushModel(knife->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0];
        REQUIRE(brush.impl->Points().size() == 8);
        REQUIRE(brush.impl->Faces().size() == 5);
        REQUIRE(brush.impl->GetHalfedge()->GetEdges().Size() == 20);

        auto& caabb = knife->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();
        CheckAABB(aabb, sm::vec3(-0.5f, 0, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep below")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepBelow);

        eval.Update();

        auto brush_model = GetBrushModel(knife->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0];
        REQUIRE(brush.impl->Points().size() == 8);
        REQUIRE(brush.impl->Faces().size() == 5);
        REQUIRE(brush.impl->GetHalfedge()->GetEdges().Size() == 20);

        auto& caabb = knife->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();
        CheckAABB(aabb, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0, 0.5f));
    }
}

TEST_CASE("boolean")
{
    test::init();

    evt::Evaluator eval;

    auto box0 = std::make_shared<evt::node::Box>();
    const sm::vec3 size0(4, 1, 4);
    box0->SetSize(size0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<evt::node::Box>();
    const sm::vec3 size1(1, 2, 1);
    box1->SetSize(size1);
    eval.AddNode(box1);

    auto boolean = std::make_shared<evt::node::Boolean>();
    eval.AddNode(boolean);

    eval.Connect({ box0, 0 }, { boolean, evt::node::Boolean::IDX_A });
    eval.Connect({ box1, 0 }, { boolean, evt::node::Boolean::IDX_B });

    SECTION("union")
    {
    }

    SECTION("intersect")
    {
        boolean->SetOperator(evt::node::Boolean::Operator::Intersect);

        eval.Update();

        auto brush_model = GetBrushModel(boolean->GetSceneNode());
        auto& brushes = brush_model->GetBrushes();
        REQUIRE(brushes.size() == 1);
        auto& brush = brushes[0];
        REQUIRE(brush.impl->Points().size() == 8);
        REQUIRE(brush.impl->Faces().size() == 6);
        REQUIRE(brush.impl->GetHalfedge()->GetEdges().Size() == 24);

        auto& caabb = boolean->GetSceneNode()->GetUniqueComp<n3::CompAABB>();
        auto& aabb = caabb.GetAABB();
        CheckAABB(aabb, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("subtract")
    {
    }
}