#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>

#include <everything/node/Add.h>
#include <everything/node/Boolean.h>
#include <everything/node/Knife.h>
#include <everything/node/PolyExtrude.h>

#include <everything/node/Box.h>
#include <everything/node/Line.h>
#include <everything/node/GroupCreate.h>

#include <catch/catch.hpp>

TEST_CASE("add")
{
    test::init();

    evt::Evaluator eval;

    SECTION("add to brush")
    {
        auto box = std::make_shared<evt::node::Box>();
        eval.AddNode(box);

        auto add = std::make_shared<evt::node::Add>();
        eval.AddNode(add);

        add->SetPoints({ {1, 2, 3}, {4, 5, 6} });

        eval.Connect({ box, 0 }, { add, 0 });

        eval.Update();

        test::check_points_num(add, 10);
    }

    SECTION("add to shape")
    {
        auto line = std::make_shared<evt::node::Line>();
        eval.AddNode(line);

        auto add = std::make_shared<evt::node::Add>();
        eval.AddNode(add);

        add->SetPoints({ {1, 2, 3}, {4, 5, 6} });

        eval.Connect({ line, 0 }, { add, 0 });

        eval.Update();

        test::check_points_num(add, 4);
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

        test::check_points_num(boolean, 8);
        //test::check_edges_num(boolean, 24);
        test::check_faces_num(boolean, 6);

        test::check_aabb(boolean, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("subtract")
    {
    }
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

        test::check_points_num(knife, 12);
        //test::check_edges_num(knife, 40);
        test::check_faces_num(knife, 10);

        test::check_aabb(knife, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep above")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepAbove);

        eval.Update();

        test::check_points_num(knife, 8);
        //test::check_edges_num(knife, 20);
        test::check_faces_num(knife, 5);

        test::check_aabb(knife, sm::vec3(-0.5f, 0, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep below")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepBelow);

        eval.Update();

        test::check_points_num(knife, 8);
        //test::check_edges_num(knife, 20);
        test::check_faces_num(knife, 5);

        test::check_aabb(knife, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0, 0.5f));
    }
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

        auto h_sz = size * 0.5f;
        test::check_aabb(extrude, -h_sz - sm::vec3(dist, dist, dist), h_sz + sm::vec3(dist, dist, dist));
    }

    SECTION("extrude one face")
    {
        auto group = std::make_shared<evt::node::GroupCreate>();
        const std::string name("test");
        group->SetGroupName(name);
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

        auto h_sz = size * 0.5f;
        test::check_aabb(extrude, -h_sz, sm::vec3(h_sz.x, h_sz.y + dist, h_sz.z));
    }
}
