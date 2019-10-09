#include "utility.h"

#include <sop/Evaluator.h>

#include <sop/node/AttributeCreate.h>
#include <sop/node/Measure.h>
#include <sop/node/Sort.h>

#include <sop/node/Add.h>
#include <sop/node/Box.h>

#include <catch/catch.hpp>

TEST_CASE("attribute create")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({ {1, 1, 1} });
    eval.AddNode(add);

    auto attr_create = std::make_shared<sop::node::AttributeCreate>();
    attr_create->AddAttr("new_attr", sop::GeoAttrClass::Point, sop::GeoAttrType::Float, sop::VarValue(0.1f));
    eval.AddNode(attr_create);

    eval.Connect({ add, 0 }, { attr_create, 0 });

    eval.Update();

    test::check_attr_count(attr_create, sop::GeoAttrClass::Point, "new_attr", 1);
    test::check_attr_value(attr_create, sop::GeoAttrClass::Point, "new_attr", 0, sop::Variable(0.1f));
}

TEST_CASE("measure box")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto measure = std::make_shared<sop::node::Measure>();
    eval.AddNode(measure);

    eval.Connect({ box, 0 }, { measure, 0 });

    SECTION("perimeter")
    {
        measure->SetMesureType(sop::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(measure, sop::GeoAttrClass::Primitive, "perimeter", 6);
        test::check_attr_value(measure, sop::GeoAttrClass::Primitive, "perimeter", 3, sop::Variable(4.0f));
    }

    SECTION("area")
    {
        measure->SetMesureType(sop::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(measure, sop::GeoAttrClass::Primitive, "area", 6);
        test::check_attr_value(measure, sop::GeoAttrClass::Primitive, "area", 2, sop::Variable(1.0f));
    }
}

TEST_CASE("measure polyline")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 0, 2, 1 },
    });
    eval.AddNode(add);

    auto measure = std::make_shared<sop::node::Measure>();
    eval.AddNode(measure);

    eval.Connect({ add, 0 }, { measure, 0 });

    SECTION("perimeter")
    {
        measure->SetMesureType(sop::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(measure, sop::GeoAttrClass::Primitive, "perimeter", 1);
        test::check_attr_value(measure, sop::GeoAttrClass::Primitive, "perimeter", 0, sop::Variable(3.0f));
    }

    SECTION("area")
    {
        measure->SetMesureType(sop::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(measure, sop::GeoAttrClass::Primitive, "area", 1);
        test::check_attr_value(measure, sop::GeoAttrClass::Primitive, "area", 0, sop::Variable(0.0f));
    }
}

TEST_CASE("measure rename")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto measure = std::make_shared<sop::node::Measure>();
    eval.AddNode(measure);

    eval.Connect({ box, 0 }, { measure, 0 });

    SECTION("perimeter")
    {
        measure->SetMesureName("test0");
        measure->SetMesureType(sop::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(measure, sop::GeoAttrClass::Primitive, "test0", 6);
        test::check_attr_value(measure, sop::GeoAttrClass::Primitive, "test0", 3, sop::Variable(4.0f));
    }

    SECTION("area")
    {
        measure->SetMesureName("test1");
        measure->SetMesureType(sop::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(measure, sop::GeoAttrClass::Primitive, "test1", 6);
        test::check_attr_value(measure, sop::GeoAttrClass::Primitive, "test1", 2, sop::Variable(1.0f));
    }
}

TEST_CASE("sort")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({
        { 0, 0, 0 },
        { 1, -2, 3 },
        { -1, 2, -3 },
    });
    eval.AddNode(add);

    auto sort = std::make_shared<sop::node::Sort>();
    eval.AddNode(sort);

    eval.Connect({ add, 0 }, { sort, 0 });

    SECTION("no sort")
    {
        sort->SetKey(sop::node::Sort::Key::NoChange);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(0, 0, 0));
        test::check_point(sort, 1, sm::vec3(1, -2, 3));
        test::check_point(sort, 2, sm::vec3(-1, 2, -3));
    }

    SECTION("sort x")
    {
        sort->SetKey(sop::node::Sort::Key::X);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(-1, 2, -3));
        test::check_point(sort, 1, sm::vec3(0, 0, 0));
        test::check_point(sort, 2, sm::vec3(1, -2, 3));
    }

    SECTION("sort y")
    {
        sort->SetKey(sop::node::Sort::Key::Y);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(1, -2, 3));
        test::check_point(sort, 1, sm::vec3(0, 0, 0));
        test::check_point(sort, 2, sm::vec3(-1, 2, -3));
    }

    SECTION("sort z")
    {
        sort->SetKey(sop::node::Sort::Key::Z);

        eval.Update();

        test::check_point(sort, 0, sm::vec3(-1, 2, -3));
        test::check_point(sort, 1, sm::vec3(0, 0, 0));
        test::check_point(sort, 2, sm::vec3(1, -2, 3));
    }
}