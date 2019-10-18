#include "utility.h"

#include <sop/Evaluator.h>

#include <sop/node/AttributeCreate.h>
#include <sop/node/AttributeTransfer.h>
#include <sop/node/AttributeWrangle.h>
#include <sop/node/Measure.h>
#include <sop/node/Sort.h>

#include <sop/node/Add.h>
#include <sop/node/Box.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/Blast.h>
#include <sop/node/Normal.h>

#include <catch/catch.hpp>

TEST_CASE("attribute create")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({ {1, 1, 1} });
    eval.AddNode(add);

    auto attr_create = std::make_shared<sop::node::AttributeCreate>();
    std::vector<sop::node::AttributeCreate::Item> items;
    items.emplace_back("new_attr", sop::GeoAttrType::Float, sop::GeoAttrClass::Point, sop::VarValue(0.1f));
    attr_create->SetAttrItems(items);
    eval.AddNode(attr_create);

    eval.Connect({ add, 0 }, { attr_create, 0 });

    eval.Update();

    test::check_attr_count(attr_create, sop::GeoAttrClass::Point, "new_attr", 1);
    test::check_attr_value(attr_create, sop::GeoAttrClass::Point, "new_attr", 0, sop::Variable(0.1f));
}

TEST_CASE("attribute transfer")
{
    test::init();

    sop::Evaluator eval;

    auto box_t = std::make_shared<sop::node::Box>();
    eval.AddNode(box_t);

    auto box_f = std::make_shared<sop::node::Box>();
    eval.AddNode(box_f);

    auto attr_transfer = std::make_shared<sop::node::AttributeTransfer>();
    eval.AddNode(attr_transfer);

    SECTION("add new one")
    {
        auto attr_create = std::make_shared<sop::node::AttributeCreate>();
        std::vector<sop::node::AttributeCreate::Item> items;
        items.emplace_back("new_attr", sop::GeoAttrType::Float, sop::GeoAttrClass::Point, sop::VarValue(0.1f));
        attr_create->SetAttrItems(items);
        eval.AddNode(attr_create);

        eval.Connect({ box_f, 0 }, { attr_create, 0 });

        attr_transfer->SetCopyAttrs(sop::GeoAttrClass::Point, { "new_attr" });

        eval.Connect({ box_t, 0 }, { attr_transfer, sop::node::AttributeTransfer::IDX_TO_GEO });
        eval.Connect({ attr_create, 0 }, { attr_transfer, sop::node::AttributeTransfer::IDX_FROM_GEO });

        eval.Update();

        test::check_attr_count(box_t, sop::GeoAttrClass::Point, "new_attr", 0);

        test::check_attr_count(attr_transfer, sop::GeoAttrClass::Point, "new_attr", 8);
        test::check_attr_value(attr_transfer, sop::GeoAttrClass::Point, "new_attr", 0, sop::Variable(0.1f));
    }

    SECTION("change old")
    {
        auto attr_create_f = std::make_shared<sop::node::AttributeCreate>();
        {
            std::vector<sop::node::AttributeCreate::Item> items;
            items.emplace_back("new_attr", sop::GeoAttrType::Float, sop::GeoAttrClass::Point, sop::VarValue(0.1f));
            attr_create_f->SetAttrItems(items);
        }
        eval.AddNode(attr_create_f);

        auto attr_create_t = std::make_shared<sop::node::AttributeCreate>();
        {
            std::vector<sop::node::AttributeCreate::Item> items;
            items.emplace_back("new_attr", sop::GeoAttrType::Float, sop::GeoAttrClass::Point, sop::VarValue(0.2f));
            attr_create_t->SetAttrItems(items);
        }
        eval.AddNode(attr_create_t);

        eval.Connect({ box_f, 0 }, { attr_create_f, 0 });
        eval.Connect({ box_t, 0 }, { attr_create_t, 0 });

        attr_transfer->SetCopyAttrs(sop::GeoAttrClass::Point, { "new_attr" });

        eval.Connect({ attr_create_t, 0 }, { attr_transfer, sop::node::AttributeTransfer::IDX_TO_GEO });
        eval.Connect({ attr_create_f, 0 }, { attr_transfer, sop::node::AttributeTransfer::IDX_FROM_GEO });

        eval.Update();

        test::check_attr_count(attr_create_t, sop::GeoAttrClass::Point, "new_attr", 8);
        test::check_attr_value(attr_create_t, sop::GeoAttrClass::Point, "new_attr", 0, sop::Variable(0.2f));

        test::check_attr_count(attr_transfer, sop::GeoAttrClass::Point, "new_attr", 8);
        test::check_attr_value(attr_transfer, sop::GeoAttrClass::Point, "new_attr", 0, sop::Variable(0.1f));
    }
}

TEST_CASE("attribute wrangle")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDeleteNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto normal = std::make_shared<sop::node::Normal>();
    normal->SetAttrAddTo(sop::GeoAttrClass::Point);
    eval.AddNode(normal);

    eval.Connect({ blast, 0 }, { normal, 0 });

    auto attr_wrangle = std::make_shared<sop::node::AttributeWrangle>();
    attr_wrangle->SetVexExpr("v@up=v@N;");
    eval.AddNode(attr_wrangle);

    eval.Connect({ normal, 0 }, { attr_wrangle, 0 });

    eval.Update();

    test::check_attr_count(normal, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 4);
    test::check_attr_value(normal, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 2, sop::Variable(sm::vec3(0, 1, 0)));
    test::check_attr_count(attr_wrangle, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 4);
    test::check_attr_value(attr_wrangle, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 2, sop::Variable(sm::vec3(0, 1, 0)));
    test::check_attr_count(attr_wrangle, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_UP], 4);
    test::check_attr_value(attr_wrangle, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_UP], 1, sop::Variable(sm::vec3(0, 1, 0)));
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