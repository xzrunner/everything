#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/GeometryImpl.h>
#include <sop/GeoAttrDefine.h>

#include <sop/node/Carve.h>

#include <sop/node/Line.h>
#include <sop/node/Color.h>
#include <sop/node/Box.h>
#include <sop/node/Normal.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/GroupExpression.h>
#include <sop/node/Blast.h>
#include <sop/node/Add.h>

#include <catch/catch.hpp>

TEST_CASE("carve")
{
    test::init();

    sop::Evaluator eval;

    auto line = std::make_shared<sop::node::Line>();

    sm::vec3 ori(0, 0, 0);
    sm::vec3 dir(0, 1, 0);
    float len = 10.0f;
    size_t num = 3;

    line->SetOrigin(ori);
    line->SetDirection(dir);
    line->SetLength(len);
    line->SetPoints(num);

    eval.AddNode(line);

    auto color = std::make_shared<sop::node::Color>();
    color->SetAttrAddTo(sop::GeoAttrType::Point);
    color->SetColor({ 0.3f, 0.4f, 0.5f });
    eval.AddNode(color);

    eval.Connect({ line, 0 }, { color, 0 });

    auto carve = std::make_shared<sop::node::Carve>();
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

        test::check_attr_count(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 3);
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(0, 0, 0)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 1, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    }

    SECTION("second u")
    {
        carve->SetSecondU(0.7f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 0, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 7, 0));

        test::check_attr_count(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 3);
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 1, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0, 0, 0)));
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

        test::check_attr_count(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 3);
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(0, 0, 0)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 1, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0, 0, 0)));
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

        test::check_attr_count(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 3);
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(0, 0, 0)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 1, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0, 0, 0)));
    }

    SECTION("expr fit")
    {
        auto& props = const_cast<sop::NodePropsMgr&>(carve->GetProps());
        REQUIRE(props.Size() == sop::node::Carve::MAX_BUILD_IN_PROP);

        props.SetExpr(sop::node::Carve::FIRST_U,  "fit(3, 0, 10, 0, 1)");
        props.SetExpr(sop::node::Carve::SECOND_U, "1 - ch(\"domainu1\")");

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 3, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 7, 0));

        test::check_attr_count(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 3);
    }
}

TEST_CASE("carve normal")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("Top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->EnableKeepByNormals({ 0, 1, 0 }, 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("Top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDeleteNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto normal = std::make_shared<sop::node::Normal>();
    normal->SetAttrAddTo(sop::GeoAttrType::Point);
    eval.AddNode(normal);

    eval.Connect({ blast, 0 }, { normal, 0 });

    auto group_expr = std::make_shared<sop::node::GroupExpression>();
    group_expr->SetGroupType(sop::GroupType::Points);
    sop::node::GroupExpression::Instance inst;
    inst.group_name = "test0";
    inst.expr_str = "@P.x < 0";
    inst.merge_op = sop::GroupMerge::Union;
    group_expr->AddInstance(inst);
    eval.AddNode(group_expr);

    eval.Connect({ normal, 0 }, { group_expr, 0 });

    auto blast2 = std::make_shared<sop::node::Blast>();
    blast2->SetGroupName("test0");
    blast2->SetGroupType(sop::GroupType::GuessFromGroup);
    blast2->SetDeleteNonSelected(true);
    eval.AddNode(blast2);

    eval.Connect({ group_expr, 0 }, { blast2, 0 });

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    eval.Connect({ blast2, 0 }, { add, 0 });

    auto carve = std::make_shared<sop::node::Carve>();
    eval.AddNode(carve);

    eval.Connect({ add, 0 }, { carve, 0 });

    SECTION("begin")
    {
        carve->SetFirstU(0.25f);

        eval.Update();

        test::check_attr_count(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 2);
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 1, 0)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1, sop::Variable(sm::vec3(0, 1, 0)));
    }

    SECTION("end")
    {
        carve->SetSecondU(0.75f);

        eval.Update();

        test::check_attr_count(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 2);
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 1, 0)));
        test::check_attr_value(carve, sop::GeoAttrType::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1, sop::Variable(sm::vec3(0, 1, 0)));
    }
}