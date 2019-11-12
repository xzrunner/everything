#include "utility.h"

#include <sop/Evaluator.h>

#include <sop/node/ObjectMerge.h>

#include <sop/node/Box.h>

#include <catch/catch.hpp>

TEST_CASE("object merge")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    const sm::vec3 size0(1, 2, 3);
    box0->SetSize(size0);
    const sm::vec3 pos0(21, 22, 23);
    box0->SetCenter(pos0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<sop::node::Box>();
    const sm::vec3 size1(11, 12, 13);
    box1->SetSize(size1);
    const sm::vec3 pos1(121, 122, 123);
    box1->SetCenter(pos1);
    eval.AddNode(box1);

    auto box2 = std::make_shared<sop::node::Box>();
    const sm::vec3 size2(21, 22, 23);
    box2->SetSize(size2);
    const sm::vec3 pos2(221, 222, 223);
    box2->SetCenter(pos2);
    eval.AddNode(box2);

    auto merge = std::make_shared<sop::node::ObjectMerge>();
    merge->SetObjects({ box0, box1, box2 });
    eval.AddNode(merge);

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