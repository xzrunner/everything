#include "utility.h"

#include <everything/Evaluator.h>

#include <everything/node/Sort.h>

#include <everything/node/Add.h>

#include <catch/catch.hpp>

TEST_CASE("sort")
{
    test::init();

    evt::Evaluator eval;

    auto add = std::make_shared<evt::node::Add>();
    add->SetPoints({
        { 0, 0, 0 },
        { 1, -2, 3 },
        { -1, 2, -3 },
    });
    eval.AddNode(add);

    auto sort = std::make_shared<evt::node::Sort>();
    eval.AddNode(sort);

    eval.Connect({ add, 0 }, { sort, 0 });

    SECTION("no sort")
    {
        sort->SetKey(evt::node::Sort::Key::NoChange);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(0, 0, 0));
        test::check_point(sort, 1, sm::vec3(1, -2, 3));
        test::check_point(sort, 2, sm::vec3(-1, 2, -3));
    }

    SECTION("sort x")
    {
        sort->SetKey(evt::node::Sort::Key::X);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(-1, 2, -3));
        test::check_point(sort, 1, sm::vec3(0, 0, 0));
        test::check_point(sort, 2, sm::vec3(1, -2, 3));
    }

    SECTION("sort y")
    {
        sort->SetKey(evt::node::Sort::Key::Y);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(1, -2, 3));
        test::check_point(sort, 1, sm::vec3(0, 0, 0));
        test::check_point(sort, 2, sm::vec3(-1, 2, -3));
    }

    SECTION("sort z")
    {
        sort->SetKey(evt::node::Sort::Key::Z);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(-1, 2, -3));
        test::check_point(sort, 1, sm::vec3(0, 0, 0));
        test::check_point(sort, 2, sm::vec3(1, -2, 3));
    }
}