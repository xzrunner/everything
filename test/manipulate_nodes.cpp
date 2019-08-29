#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>

#include <everything/node/Delete.h>
#include <everything/node/Transform.h>

#include <everything/node/Box.h>

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
    del->SetFilterExp("@P.y < 0");
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

    auto trans = std::make_shared<evt::node::Transform>();
    eval.AddNode(trans);

    eval.Connect({ box, 0 }, { trans, 0 });

    SECTION("scale")
    {
        const sm::vec3 scale(2, 3, 4);
        trans->SetScale(scale);

        eval.Update();

        test::check_aabb(trans, -h_sz * scale, h_sz * scale);
    }

    SECTION("rotate x")
    {
        const sm::vec3 rot(90, 0, 0);
        trans->SetRotate(rot);

        eval.Update();

        test::check_aabb(trans, sm::vec3(-h_sz.x, -h_sz.z, -h_sz.y), sm::vec3(h_sz.x, h_sz.z, h_sz.y));
    }

    SECTION("rotate y")
    {
        const sm::vec3 rot(0, 90, 0);
        trans->SetRotate(rot);

        eval.Update();

        test::check_aabb(trans, sm::vec3(-h_sz.z, -h_sz.y, -h_sz.x), sm::vec3(h_sz.z, h_sz.y, h_sz.x));
    }

    SECTION("rotate z")
    {
        const sm::vec3 rot(0, 0, 90);
        trans->SetRotate(rot);

        eval.Update();

        test::check_aabb(trans, sm::vec3(-h_sz.y, -h_sz.x, -h_sz.z), sm::vec3(h_sz.y, h_sz.x, h_sz.z));
    }

    SECTION("translate")
    {
        const sm::vec3 off(1.1f, 2.2f, 3.3f);
        trans->SetTranslate(off);

        eval.Update();

        test::check_aabb(trans, -h_sz + off, h_sz + off);
    }
}