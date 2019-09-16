#include "utility.h"

#include <everything/Evaluator.h>

#include <everything/node/Measure.h>
#include <everything/node/Sort.h>

#include <everything/node/Add.h>
#include <everything/node/Box.h>

#include <catch/catch.hpp>

TEST_CASE("measure box")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto measure = std::make_shared<evt::node::Measure>();
    eval.AddNode(measure);

    eval.Connect({ box, 0 }, { measure, 0 });

    SECTION("perimeter")
    {
        measure->SetMesureType(evt::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(measure, evt::GeoAttrType::Primitive, "perimeter", 6);
        test::check_attr_value(measure, evt::GeoAttrType::Primitive, "perimeter", 3, evt::Variable(4.0f));
    }

    SECTION("area")
    {
        measure->SetMesureType(evt::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(measure, evt::GeoAttrType::Primitive, "area", 6);
        test::check_attr_value(measure, evt::GeoAttrType::Primitive, "area", 2, evt::Variable(1.0f));
    }
}

TEST_CASE("measure polyline")
{
    test::init();

    evt::Evaluator eval;

    auto add = std::make_shared<evt::node::Add>();
    add->SetPoints({
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 0, 2, 1 },
    });
    eval.AddNode(add);

    auto measure = std::make_shared<evt::node::Measure>();
    eval.AddNode(measure);

    eval.Connect({ add, 0 }, { measure, 0 });

    SECTION("perimeter")
    {
        measure->SetMesureType(evt::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(measure, evt::GeoAttrType::Primitive, "perimeter", 1);
        test::check_attr_value(measure, evt::GeoAttrType::Primitive, "perimeter", 0, evt::Variable(3.0f));
    }

    SECTION("area")
    {
        measure->SetMesureType(evt::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(measure, evt::GeoAttrType::Primitive, "area", 1);
        test::check_attr_value(measure, evt::GeoAttrType::Primitive, "area", 0, evt::Variable(0.0f));
    }
}

TEST_CASE("measure rename")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto measure = std::make_shared<evt::node::Measure>();
    eval.AddNode(measure);

    eval.Connect({ box, 0 }, { measure, 0 });

    SECTION("perimeter")
    {
        measure->SetMesureName("test0");
        measure->SetMesureType(evt::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(measure, evt::GeoAttrType::Primitive, "test0", 6);
        test::check_attr_value(measure, evt::GeoAttrType::Primitive, "test0", 3, evt::Variable(4.0f));
    }

    SECTION("area")
    {
        measure->SetMesureName("test1");
        measure->SetMesureType(evt::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(measure, evt::GeoAttrType::Primitive, "test1", 6);
        test::check_attr_value(measure, evt::GeoAttrType::Primitive, "test1", 2, evt::Variable(1.0f));
    }
}

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