#include "utility.h"

#include <everything/TreeContext.h>
#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>

#include <everything/node/Carve.h>

#include <everything/node/Line.h>

#include <catch/catch.hpp>

TEST_CASE("carve")
{
    test::init();

    evt::Evaluator eval;

    auto line = std::make_shared<evt::node::Line>();

    sm::vec3 ori(0, 0, 0);
    sm::vec3 dir(0, 1, 0);
    float len = 10.0f;
    size_t num = 3;

    line->SetOrigin(ori);
    line->SetDirection(dir);
    line->SetLength(len);
    line->SetPoints(num);

    eval.AddNode(line);

    auto carve = std::make_shared<evt::node::Carve>();
    eval.AddNode(carve);

    eval.Connect({ line, 0 }, { carve, 0 });

    SECTION("first u")
    {
        carve->SetFirstU(0.2f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_pos(carve, 0, sm::vec3(0, 2, 0));
        test::check_pos(carve, 1, sm::vec3(0, 5, 0));
        test::check_pos(carve, 2, sm::vec3(0, 10, 0));
    }

    SECTION("second u")
    {
        carve->SetSecondU(0.7f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_pos(carve, 0, sm::vec3(0, 0, 0));
        test::check_pos(carve, 1, sm::vec3(0, 5, 0));
        test::check_pos(carve, 2, sm::vec3(0, 7, 0));
    }

    SECTION("first and second u")
    {
        carve->SetFirstU(0.3f);
        carve->SetSecondU(0.6f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_pos(carve, 0, sm::vec3(0, 3, 0));
        test::check_pos(carve, 1, sm::vec3(0, 5, 0));
        test::check_pos(carve, 2, sm::vec3(0, 6, 0));
    }

    SECTION("first and second u inv")
    {
        carve->SetFirstU(0.6f);
        carve->SetSecondU(0.3f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_pos(carve, 0, sm::vec3(0, 3, 0));
        test::check_pos(carve, 1, sm::vec3(0, 5, 0));
        test::check_pos(carve, 2, sm::vec3(0, 6, 0));
    }
}