#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/Geometry.h>
#include <everything/TreeContext.h>

#include <everything/node/Box.h>
#include <everything/node/Curve.h>
#include <everything/node/Line.h>

#include <catch/catch.hpp>

TEST_CASE("box")
{
    test::init();

    auto box = std::make_shared<evt::node::Box>();

    const sm::vec3 sz(2, 3, 4);
    auto h_sz = sz * 0.5f;
    box->SetSize(sz);
    box->Execute(evt::TreeContext());

    test::check_points_num(box, 8);
    //test::check_edges_num(box, 24);
    test::check_faces_num(box, 6);

    test::check_aabb(box, -h_sz, h_sz);

    const sm::vec3 off(10, 11, 12);
    box->SetCenter(off);
    box->Execute(evt::TreeContext());
    test::check_aabb(box, -h_sz + off, h_sz + off);

    const sm::vec3 scale(5, 6, 7);
    box->SetScale(scale);
    box->Execute(evt::TreeContext());
    test::check_aabb(box, -h_sz * scale + off, h_sz * scale + off);
}

TEST_CASE("curve")
{
    test::init();

    auto curve = std::make_shared<evt::node::Curve>();

    SECTION("no loop")
    {
        std::vector<sm::vec3> vertices = {
            { 0, 1, 0 },
            { 4, 1, 0 },
            { 4, 0, 2 },
        };
        curve->SetVertices(vertices);

        curve->Execute(evt::TreeContext());

        test::check_points_num(curve, 3);
        test::check_pos(curve, 1, sm::vec3(4, 1, 0));
        test::check_aabb(curve, sm::vec3(0, 0, 0), sm::vec3(4, 1, 2));
    }

    SECTION("loop")
    {
        std::vector<sm::vec3> vertices = {
            { 0, 1, 0 },
            { 4, 1, 0 },
            { 4, 0, 2 },
            { 0, 1, 0 },
        };
        curve->SetVertices(vertices);

        curve->Execute(evt::TreeContext());

        test::check_points_num(curve, 3);
        test::check_pos(curve, 1, sm::vec3(4, 1, 0));
        test::check_aabb(curve, sm::vec3(0, 0, 0), sm::vec3(4, 1, 2));
    }
}

TEST_CASE("line")
{
    test::init();

    auto line = std::make_shared<evt::node::Line>();

    sm::vec3 ori(1, 2, 3);
    sm::vec3 dir(11, 12, 13);
    float len = 11.0f;
    size_t num = 7;

    line->SetOrigin(ori);
    line->SetDirection(dir);
    line->SetLength(len);
    line->SetPoints(num);

    line->Execute(evt::TreeContext());

    auto pos4 = ori + dir * len / static_cast<float>(num - 1) * 4;
    test::check_pos(line, 4, pos4);
    test::check_aabb(line, ori, ori + dir * len);
}