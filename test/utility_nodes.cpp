#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>

#include <everything/node/Blast.h>
#include <everything/node/CopyToPoints.h>
#include <everything/node/ForeachPrimBegin.h>
#include <everything/node/ForeachPrimEnd.h>
#include <everything/node/GroupCreate.h>
#include <everything/node/Merge.h>
#include <everything/node/Switch.h>

#include <everything/node/Box.h>
#include <everything/node/PolyExtrude.h>
#include <everything/node/Delete.h>
#include <everything/node/Add.h>
#include <everything/node/Carve.h>

#include <catch/catch.hpp>

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
    group->SetGroupName(name);
    group->SetGroupType(evt::GroupType::Primitives);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<evt::node::Blast>();
    blast->SetGroupName(name);
    blast->SetGroupType(evt::GroupType::GuessFromGroup);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    SECTION("del selected")
    {
        eval.Update();

        test::check_faces_num(blast, 5);
        test::check_group_num(blast, name, 0);
    }

    SECTION("del non selected")
    {
        blast->SetDeleteNonSelected(true);

        eval.Update();

        test::check_faces_num(blast, 1);
        test::check_group_num(blast, name, 1);
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

    auto h_src_sz = src_size * 0.5f;
    auto h_tar_sz = target_size * 0.5f;
    test::check_aabb(copy, src_pos + target_pos - h_src_sz - h_tar_sz, src_pos + target_pos + h_src_sz + h_tar_sz);
}

TEST_CASE("copy to points with points dir")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    box->SetSize({ 1, 2, 4 });
    eval.AddNode(box);

    auto copy = std::make_shared<evt::node::CopyToPoints>();
    copy->EnableUsePointDir(true);
    eval.AddNode(copy);

    eval.Connect({ box, 0 }, { copy, evt::node::CopyToPoints::IDX_SRC_PRIM });

    SECTION("shape")
    {
        auto line = std::make_shared<evt::node::Add>();
        line->SetPoints({
            { 0, 0, 0 },
            { 2, 4, 8 }
        });
        eval.AddNode(line);

        eval.Connect({ line, 0 }, { copy, evt::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -0.5f, -1, -2 }, { 2.5f, 5, 10 });
    }

    SECTION("box")
    {
        auto to_box = std::make_shared<evt::node::Box>();
        to_box->SetSize({ 6, 6, 6 });
        eval.AddNode(to_box);

        eval.Connect({ to_box, 0 }, { copy, evt::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -5.04904f, -5.04904f, -5.02073f }, { 5.04904f, 5.04904f, 5.02073f });
    }

    SECTION("plane")
    {
        auto to_box = std::make_shared<evt::node::Box>();
        to_box->SetSize({ 6, 6, 6 });
        eval.AddNode(to_box);

        auto group = std::make_shared<evt::node::GroupCreate>();
        group->SetGroupName("Top");
        group->SetGroupType(evt::GroupType::Primitives);
        group->EnableKeepByNormals({ 0, 1, 0 }, 10);
        eval.AddNode(group);

        eval.Connect({ to_box, 0 }, { group, 0 });

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("Top");
        blast->SetGroupType(evt::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group, 0 }, { blast, 0 });

        eval.Connect({ blast, 0 }, { copy, evt::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -3.5f, 1, -4 }, { 3.5f, 5, 4 });
    }

    SECTION("plane with normal")
    {
        auto to_box = std::make_shared<evt::node::Box>();
        to_box->SetSize({ 6, 6, 6 });
        eval.AddNode(to_box);

        auto normal = std::make_shared<evt::node::Normal>();
        normal->SetAttrAddTo(evt::GeoAttrType::Point);
        eval.AddNode(normal);

        eval.Connect({ to_box, 0 }, { normal, 0 });

        auto group = std::make_shared<evt::node::GroupCreate>();
        group->SetGroupName("Top");
        group->SetGroupType(evt::GroupType::Primitives);
        group->EnableKeepByNormals({ 0, 1, 0 }, 10);
        eval.AddNode(group);

        eval.Connect({ normal, 0 }, { group, 0 });

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("Top");
        blast->SetGroupType(evt::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group, 0 }, { blast, 0 });

        eval.Connect({ blast, 0 }, { copy, evt::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -5.04904f, 0.950962f, -5.02073f }, { 5.04904f, 5.04904f, 5.02073f });
    }
}

TEST_CASE("foreach primitive")
{
    test::init();

    evt::Evaluator eval;

    auto src_box = std::make_shared<evt::node::Box>();
    eval.AddNode(src_box);

    // cut top face
    auto top_face_group = std::make_shared<evt::node::GroupCreate>();
    top_face_group->SetGroupName("top_face");
    top_face_group->SetGroupType(evt::GroupType::Primitives);
    top_face_group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(top_face_group);
    evt::make_connecting({ src_box, 0 }, { top_face_group, 0 });
    auto top_face_blast = std::make_shared<evt::node::Blast>();
    top_face_blast->SetGroupName("top_face");
    top_face_blast->SetGroupType(evt::GroupType::GuessFromGroup);
    eval.AddNode(top_face_blast);
    evt::make_connecting({ top_face_group, 0 }, { top_face_blast, 0 });

    // cut bottom face
    auto btm_face_group = std::make_shared<evt::node::GroupCreate>();
    btm_face_group->SetGroupName("btm_face");
    btm_face_group->SetGroupType(evt::GroupType::Primitives);
    btm_face_group->EnableKeepByNormals(sm::vec3(0, -1, 0), 10);
    eval.AddNode(btm_face_group);
    evt::make_connecting({ top_face_blast, 0 }, { btm_face_group, 0 });
    auto btm_face_blast = std::make_shared<evt::node::Blast>();
    btm_face_blast->SetGroupName("btm_face");
    btm_face_blast->SetGroupType(evt::GroupType::GuessFromGroup);
    eval.AddNode(btm_face_blast);
    evt::make_connecting({ btm_face_group, 0 }, { btm_face_blast, 0 });

    auto foreach_begin = std::make_shared<evt::node::ForeachPrimBegin>();
    eval.AddNode(foreach_begin);

    evt::make_connecting({ btm_face_blast, 0 }, { foreach_begin, 0 });

    auto del = std::make_shared<evt::node::Delete>();
    del->SetFilterExpr("@P.y > 0");
    eval.AddNode(del);

    evt::make_connecting({ foreach_begin, 0 }, { del, 0 });

    auto add = std::make_shared<evt::node::Add>();
    eval.AddNode(add);

    evt::make_connecting({ del, 0 }, { add, 0 });

    auto carve = std::make_shared<evt::node::Carve>();
    carve->SetFirstU(0.25f);
    carve->SetSecondU(0.75f);
    eval.AddNode(carve);

    evt::make_connecting({ add, 0 }, { carve, 0 });

    auto foreach_end = std::make_shared<evt::node::ForeachPrimEnd>();
    eval.AddNode(foreach_end);

    evt::make_connecting({ carve, 0 }, { foreach_end, 0 });

    auto dst_box = std::make_shared<evt::node::Box>();
    dst_box->SetSize({ 0.1f, 0.1f, 0.1f });
    eval.AddNode(dst_box);

    auto copy = std::make_shared<evt::node::CopyToPoints>();
    eval.AddNode(copy);

    eval.Connect({ dst_box, 0 },     { copy, evt::node::CopyToPoints::IDX_SRC_PRIM });
    eval.Connect({ foreach_end, 0 }, { copy, evt::node::CopyToPoints::IDX_TARGET_POS });

    eval.Update();

    test::check_points_num(copy, 64);
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
    merge->AddInputPorts(3 - merge->GetImports().size());
    eval.AddNode(merge);

    eval.Connect({ box0, 0 }, { merge, 0 });
    eval.Connect({ box1, 0 }, { merge, 1 });
    eval.Connect({ box2, 0 }, { merge, 2 });

    eval.Update();

    test::check_points_num(merge, 24);
    test::check_faces_num(merge, 18);

    sm::vec3 aabb_min, aabb_max;
    aabb_min.x = std::min(pos0.x - size0.x * 0.5f, std::min(pos1.x - size1.x * 0.5f, pos2.x - size2.x * 0.5f));
    aabb_min.y = std::min(pos0.y - size0.y * 0.5f, std::min(pos1.y - size1.y * 0.5f, pos2.y - size2.y * 0.5f));
    aabb_min.z = std::min(pos0.z - size0.z * 0.5f, std::min(pos1.z - size1.z * 0.5f, pos2.z - size2.z * 0.5f));
    aabb_max.x = std::max(pos0.x + size0.x * 0.5f, std::max(pos1.x + size1.x * 0.5f, pos2.x + size2.x * 0.5f));
    aabb_max.y = std::max(pos0.y + size0.y * 0.5f, std::max(pos1.y + size1.y * 0.5f, pos2.y + size2.y * 0.5f));
    aabb_max.z = std::max(pos0.z + size0.z * 0.5f, std::max(pos1.z + size1.z * 0.5f, pos2.z + size2.z * 0.5f));
    test::check_aabb(merge, aabb_min, aabb_max);
}

TEST_CASE("switch")
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

    auto switch0 = std::make_shared<evt::node::Switch>();
    switch0->AddInputPorts(3 - switch0->GetImports().size());
    eval.AddNode(switch0);

    eval.Connect({ box0, 0 }, { switch0, 0 });
    eval.Connect({ box1, 0 }, { switch0, 1 });
    eval.Connect({ box2, 0 }, { switch0, 2 });

    SECTION("init sel 0")
    {
        eval.Update();

        test::check_aabb(switch0, pos0 - size0 * 0.5f, pos0 + size0 * 0.5f);
    }

    SECTION("sel 1")
    {
        switch0->SetSelected(1);

        eval.Update();

        test::check_aabb(switch0, pos1 - size1 * 0.5f, pos1 + size1 * 0.5f);
    }
}