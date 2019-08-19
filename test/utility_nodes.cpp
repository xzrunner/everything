#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/Geometry.h>

#include <everything/node/Blast.h>
#include <everything/node/CopyToPoints.h>
#include <everything/node/GroupCreate.h>
#include <everything/node/Merge.h>

#include <everything/node/Box.h>
#include <everything/node/PolyExtrude.h>

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
    group->SetName(name);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<evt::node::Blast>();
    blast->SetGroupName(name);
    blast->SetGroupType(evt::GroupType::Primitives);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    SECTION("del selected")
    {
        eval.Update();

        test::check_faces_num(blast, 5);
    }

    SECTION("del non selected")
    {
        blast->SetDeleteNonSelected(true);

        eval.Update();

        test::check_faces_num(blast, 1);
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

    test::check_points_num(merge, 16);
    test::check_faces_num(merge, 12);

    test::check_aabb(extrude, sm::vec3(-5.5f, -0.5f, -0.5f), sm::vec3(5.5f, 0.5f, 5.5f));
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