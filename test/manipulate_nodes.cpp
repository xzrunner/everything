#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>

#include <everything/node/Delete.h>
#include <everything/node/Transform.h>

#include <everything/node/Box.h>
#include <everything/node/GroupCreate.h>

#include <catch/catch.hpp>

TEST_CASE("delete")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    const sm::vec3 size(2, 3, 4);
    box->SetSize(size);
    eval.AddNode(box);

    auto del = std::make_shared<evt::node::Delete>();
    del->SetFilterExpr("@P.y < 0");
    eval.AddNode(del);

    eval.Connect({ box, 0 }, { del, 0 });

    eval.Update();

    test::check_points_num(del, 4);
}

TEST_CASE("transform")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    const sm::vec3 size(1, 2, 3);
    auto h_sz = size * 0.5f;
    box->SetSize(size);
    eval.AddNode(box);

    SECTION("scale")
    {
        auto trans = std::make_shared<evt::node::Transform>();
        eval.AddNode(trans);

        eval.Connect({ box, 0 }, { trans, 0 });

        const sm::vec3 scale(2, 3, 4);
        trans->SetScale(scale);

        eval.Update();

        test::check_aabb(trans, -h_sz * scale, h_sz * scale);
    }

    SECTION("rotate x")
    {
        auto trans = std::make_shared<evt::node::Transform>();
        eval.AddNode(trans);

        eval.Connect({ box, 0 }, { trans, 0 });

        const sm::vec3 rot(90, 0, 0);
        trans->SetRotate(rot);

        eval.Update();

        test::check_aabb(trans, sm::vec3(-h_sz.x, -h_sz.z, -h_sz.y), sm::vec3(h_sz.x, h_sz.z, h_sz.y));
    }

    SECTION("rotate y")
    {
        auto trans = std::make_shared<evt::node::Transform>();
        eval.AddNode(trans);

        eval.Connect({ box, 0 }, { trans, 0 });

        const sm::vec3 rot(0, 90, 0);
        trans->SetRotate(rot);

        eval.Update();

        test::check_aabb(trans, sm::vec3(-h_sz.z, -h_sz.y, -h_sz.x), sm::vec3(h_sz.z, h_sz.y, h_sz.x));
    }

    SECTION("rotate z")
    {
        auto trans = std::make_shared<evt::node::Transform>();
        eval.AddNode(trans);

        eval.Connect({ box, 0 }, { trans, 0 });

        const sm::vec3 rot(0, 0, 90);
        trans->SetRotate(rot);

        eval.Update();

        test::check_aabb(trans, sm::vec3(-h_sz.y, -h_sz.x, -h_sz.z), sm::vec3(h_sz.y, h_sz.x, h_sz.z));
    }

    SECTION("translate")
    {
        auto trans = std::make_shared<evt::node::Transform>();
        eval.AddNode(trans);

        eval.Connect({ box, 0 }, { trans, 0 });

        const sm::vec3 off(1.1f, 2.2f, 3.3f);
        trans->SetTranslate(off);

        eval.Update();

        test::check_aabb(trans, -h_sz + off, h_sz + off);
    }

    SECTION("group point")
    {
        auto group_create = std::make_shared<evt::node::GroupCreate>();
        group_create->SetGroupName("bottom");
        group_create->SetGroupType(evt::GroupType::Points);
        group_create->EnableBaseGroup("@P.y < 0");
        eval.AddNode(group_create);

        eval.Connect({ box, 0 }, { group_create, 0 });

        auto trans = std::make_shared<evt::node::Transform>();
        trans->SetGroupName("bottom");
        trans->SetGroupType(evt::GroupType::GuessFromGroup);
        eval.AddNode(trans);

        trans->SetTranslate(sm::vec3(5, 0, 0));

        eval.Connect({ group_create, 0 }, { trans, 0 });

        eval.Update();

        test::check_aabb(trans, -h_sz, sm::vec3(h_sz.x + 5, h_sz.y, h_sz.z));
    }

    SECTION("group vertex")
    {
        auto group_create = std::make_shared<evt::node::GroupCreate>();
        group_create->SetGroupName("bottom");
        group_create->SetGroupType(evt::GroupType::Vertices);
        group_create->EnableBaseGroup("@P.y < 0");
        eval.AddNode(group_create);

        eval.Connect({ box, 0 }, { group_create, 0 });

        auto trans = std::make_shared<evt::node::Transform>();
        trans->SetGroupName("bottom");
        trans->SetGroupType(evt::GroupType::GuessFromGroup);
        eval.AddNode(trans);

        trans->SetTranslate(sm::vec3(5, 0, 0));

        eval.Connect({ group_create, 0 }, { trans, 0 });

        eval.Update();

        test::check_aabb(trans, -h_sz, sm::vec3(h_sz.x + 5, h_sz.y, h_sz.z));
    }

    SECTION("group prim")
    {
        auto group_create = std::make_shared<evt::node::GroupCreate>();
        group_create->SetGroupName("bottom");
        group_create->SetGroupType(evt::GroupType::Primitives);
        group_create->EnableBaseGroup("@P.y < 0");
        eval.AddNode(group_create);

        eval.Connect({ box, 0 }, { group_create, 0 });

        auto trans = std::make_shared<evt::node::Transform>();
        trans->SetGroupName("bottom");
        trans->SetGroupType(evt::GroupType::GuessFromGroup);
        eval.AddNode(trans);

        trans->SetTranslate(sm::vec3(5, 0, 0));

        eval.Connect({ group_create, 0 }, { trans, 0 });

        eval.Update();

        test::check_aabb(trans, -h_sz, sm::vec3(h_sz.x + 5, h_sz.y, h_sz.z));
    }
}