#include "utility.h"

#include <everything/TreeContext.h>
#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>
#include <everything/GeoAttrName.h>

#include <everything/node/Carve.h>

#include <everything/node/Line.h>
#include <everything/node/Color.h>
#include <everything/node/Box.h>
#include <everything/node/Normal.h>
#include <everything/node/GroupCreate.h>
#include <everything/node/GroupExpression.h>
#include <everything/node/Blast.h>
#include <everything/node/Add.h>

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

    auto color = std::make_shared<evt::node::Color>();
    color->SetAttrAddTo(evt::GeoAttrType::Point);
    color->SetColor({ 0.3f, 0.4f, 0.5f });
    eval.AddNode(color);

    eval.Connect({ line, 0 }, { color, 0 });

    auto carve = std::make_shared<evt::node::Carve>();
    eval.AddNode(carve);

    eval.Connect({ color, 0 }, { carve, 0 });

    SECTION("first u")
    {
        carve->SetFirstU(0.2f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 2, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 10, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.5f));
    }

    SECTION("second u")
    {
        carve->SetSecondU(0.7f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 0, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 7, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.0f));
    }

    SECTION("first and second u")
    {
        carve->SetFirstU(0.3f);
        carve->SetSecondU(0.6f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 3, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 6, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.0f));
    }

    SECTION("first and second u inv")
    {
        carve->SetFirstU(0.6f);
        carve->SetSecondU(0.3f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 3, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 6, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.0f));
    }

    SECTION("expr fit")
    {
        auto& props = const_cast<evt::NodePropsMgr&>(carve->GetProps());
        REQUIRE(props.Size() == evt::node::Carve::MAX_BUILD_IN_PROP);

        props.SetExpr(evt::node::Carve::FIRST_U,  "fit(3, 0, 10, 0, 1)");
        props.SetExpr(evt::node::Carve::SECOND_U, "1 - ch(\"domainu1\")");

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 3, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 7, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 3);
    }
}

TEST_CASE("carve normal")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<evt::node::GroupCreate>();
    group->SetGroupName("Top");
    group->SetGroupType(evt::GroupType::Primitives);
    group->EnableKeepByNormals({ 0, 1, 0 }, 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<evt::node::Blast>();
    blast->SetGroupName("Top");
    blast->SetGroupType(evt::GroupType::GuessFromGroup);
    blast->SetDeleteNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto normal = std::make_shared<evt::node::Normal>();
    normal->SetAttrAddTo(evt::GeoAttrType::Point);
    eval.AddNode(normal);

    eval.Connect({ blast, 0 }, { normal, 0 });

    auto group_expr = std::make_shared<evt::node::GroupExpression>();
    group_expr->SetGroupType(evt::GroupType::Points);
    evt::node::GroupExpression::Instance inst;
    inst.group_name = "test0";
    inst.expr_str = "@P.x < 0";
    inst.merge_op = evt::GroupMerge::Union;
    group_expr->AddInstance(inst);
    eval.AddNode(group_expr);

    eval.Connect({ normal, 0 }, { group_expr, 0 });

    auto blast2 = std::make_shared<evt::node::Blast>();
    blast2->SetGroupName("test0");
    blast2->SetGroupType(evt::GroupType::GuessFromGroup);
    blast2->SetDeleteNonSelected(true);
    eval.AddNode(blast2);

    eval.Connect({ group_expr, 0 }, { blast2, 0 });

    auto add = std::make_shared<evt::node::Add>();
    eval.AddNode(add);

    eval.Connect({ blast2, 0 }, { add, 0 });

    auto carve = std::make_shared<evt::node::Carve>();
    eval.AddNode(carve);

    eval.Connect({ add, 0 }, { carve, 0 });

    SECTION("begin")
    {
        carve->SetFirstU(0.25f);

        eval.Update();

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 2);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, 0, evt::Variable(1.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 1, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, 1, evt::Variable(1.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, 1, evt::Variable(0.0f));
    }

    SECTION("end")
    {
        carve->SetSecondU(0.75f);

        eval.Update();

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 2);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, 0, evt::Variable(1.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 1, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, 1, evt::Variable(1.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, 1, evt::Variable(0.0f));
    }
}