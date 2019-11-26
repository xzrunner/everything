#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/EvalContext.h>

#include <sop/node/Box.h>
#include <sop/node/Geometry.h>
#include <sop/node/GroupExpression.h>
#include <sop/node/Blast.h>
#include <sop/node/Measure.h>
#include <sop/node/Line.h>
#include <sop/node/AttributeCreate.h>

#include <catch/catch.hpp>

TEST_CASE("getbox")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    box->SetSize({ 1, 2, 3 });
    box->SetCenter({ 4, 5, 6 });
    eval.AddNode(box);

    eval.Update();

    sop::EvalContext ctx(eval, *box);

    SECTION("getbbox_center")
    {
        auto v = eval.CalcExpr("getbbox_center(0)", ctx);
        REQUIRE(v.type == sop::VarType::Float3);
        auto f3 = static_cast<const float*>(v.p);
        REQUIRE(f3[0] == Approx(4));
        REQUIRE(f3[1] == Approx(5));
        REQUIRE(f3[2] == Approx(6));
    }

    SECTION("getbbox_size")
    {
        auto v = eval.CalcExpr("getbbox_size(0)", ctx);
        REQUIRE(v.type == sop::VarType::Float3);
        auto f3 = static_cast<const float*>(v.p);
        REQUIRE(f3[0] == Approx(1));
        REQUIRE(f3[1] == Approx(2));
        REQUIRE(f3[2] == Approx(3));
    }
}

TEST_CASE("point")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto attr_create = std::make_shared<sop::node::AttributeCreate>();
    attr_create->SetName("attr_create");
    attr_create->SetItemNames({ "ScaleY" });
    attr_create->SetItemClasses({ sop::GeoAttrClass::Point });
    attr_create->SetItemTypes({ sop::node::AttributeCreate::ItemType::Float });
    attr_create->SetItemValues({ sm::vec4(2, 0, 0, 0) });
    attr_create->SetItemDefaultValues({ sm::vec4(0, 0, 0, 0) });
    attr_create->SetItemFloatInfos({ sop::node::AttributeCreate::ItemFltInfo::Guess });
    attr_create->SetItemCompSize({ 1 });
    attr_create->SetItemStrings({ "" });
    eval.AddNode(attr_create);

    eval.Connect({ box, 0 }, { attr_create, 0 });

    auto box2 = std::make_shared<sop::node::Box>();
    auto& box2_parms = const_cast<sop::NodeParmsMgr&>(box2->GetParms());
    //box2_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
    //    "point(\"../attr_create/\",0,\"ScaleY\",1)", 1);
    box2_parms.SetExpr("sizey", "point(\"../attr_create/\",0,\"ScaleY\",1)");
    eval.AddNode(box2);

    eval.Update();

    test::check_aabb(box, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    test::check_aabb(box2, sm::vec3(-0.5f, -1, -0.5f), sm::vec3(0.5f, 1, 0.5f));
}

TEST_CASE("prim")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto measure = std::make_shared<sop::node::Measure>();
    measure->SetName("measure1");
    measure->SetMesureName("Perim");
    measure->SetMesureType(sop::node::Measure::Type::Perimeter);
    eval.AddNode(measure);

    eval.Connect({ box, 0 }, { measure, 0 });

    auto box2 = std::make_shared<sop::node::Box>();
    auto& box2_parms = const_cast<sop::NodeParmsMgr&>(box2->GetParms());
    //box2_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
    //    "prim(\"../measure1/\",0,\"Perim\",0)", 0);
    box2_parms.SetExpr("sizex", "prim(\"../measure1/\",0,\"Perim\",0)");
    eval.AddNode(box2);

    eval.Update();

    test::check_aabb(box2, sm::vec3(-2, -0.5f, -0.5f), sm::vec3(2, 0.5f, 0.5f));
}

TEST_CASE("attr")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group_expr = std::make_shared<sop::node::GroupExpression>();
    group_expr->SetGroupType(sop::GroupType::Points);
    eval.AddNode(group_expr);

    eval.Connect({ box, 0 }, { group_expr, 0 });

    SECTION("@P")
    {
        group_expr->SetGroupNames({ "test0" });
        group_expr->SetGroupExprs({ "@P.x < 0 && @P.y < 0 && @P.z > 0" });
        group_expr->SetGroupMergeOps({ sop::GroupMerge::Union });

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDelNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 1);
        test::check_point(blast, 0, sm::vec3(-0.5f, -0.5f, 0.5f));
    }
}


TEST_CASE("desc")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    box->SetSize({ 1, 2, 3 });
    box->SetCenter({ 4, 5, 6 });
    eval.AddNode(box);

    SECTION("calc")
    {
        eval.Update();

        sop::EvalContext ctx(eval, *box);
        auto sz_y = eval.CalcExpr("$SIZEY", ctx);
        REQUIRE(sz_y.type == sop::VarType::Float);
        REQUIRE(sz_y.f == Approx(2.0f));
    }

    // fixme: prepare geo for parm
    //SECTION("parm")
    //{
    //    auto& parms = const_cast<sop::NodePropsMgr&>(box->GetProps());
    //    parms.SetExpr(sop::node::Box::POS_Y, "$SIZEY/2");

    //    eval.Update();

    //    test::check_aabb(box, { 3.5f, 5, 4.5f }, { 4.5f, 7, 7.5f });
    //}
}