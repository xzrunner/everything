#include "utility.h"

#include <everything/TreeContext.h>
#include <everything/Evaluator.h>
#include <everything/EvalContext.h>

#include <everything/node/Box.h>
#include <everything/node/Geometry.h>
#include <everything/node/GroupExpression.h>
#include <everything/node/Blast.h>
#include <everything/node/Measure.h>
#include <everything/node/Line.h>

#include <catch/catch.hpp>

TEST_CASE("getbox")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    box->SetSize({ 1, 2, 3 });
    box->SetCenter({ 4, 5, 6 });
    eval.AddNode(box);

    eval.Update();

    evt::EvalContext ctx(eval, *box);

    SECTION("getbbox_center")
    {
        auto v = eval.CalcExpr("getbbox_center(0)", ctx);
        REQUIRE(v.type == evt::VariableType::Float3);
        auto f3 = static_cast<const float*>(v.p);
        REQUIRE(f3[0] == Approx(4));
        REQUIRE(f3[1] == Approx(5));
        REQUIRE(f3[2] == Approx(6));
    }

    SECTION("getbbox_size")
    {
        auto v = eval.CalcExpr("getbbox_size(0)", ctx);
        REQUIRE(v.type == evt::VariableType::Float3);
        auto f3 = static_cast<const float*>(v.p);
        REQUIRE(f3[0] == Approx(1));
        REQUIRE(f3[1] == Approx(2));
        REQUIRE(f3[2] == Approx(3));
    }
}

TEST_CASE("prim")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto measure = std::make_shared<evt::node::Measure>();
    measure->SetName("measure1");
    measure->SetMesureName("Perim");
    measure->SetMesureType(evt::node::Measure::Type::Perimeter);
    eval.AddNode(measure);

    eval.Connect({ box, 0 }, { measure, 0 });

    auto box2 = std::make_shared<evt::node::Box>();
    auto& box2_props = const_cast<evt::NodePropsMgr&>(box2->GetProps());
    box2_props.SetExpr(evt::node::Box::SIZE_X, "prim(\"../measure1/\",0,\"Perim\",0)");
    eval.AddNode(box2);

    eval.Update();

    test::check_aabb(box2, sm::vec3(-2, -0.5f, -0.5f), sm::vec3(2, 0.5f, 0.5f));
}

TEST_CASE("attr")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto group_expr = std::make_shared<evt::node::GroupExpression>();
    group_expr->SetGroupType(evt::GroupType::Points);
    eval.AddNode(group_expr);

    eval.Connect({ box, 0 }, { group_expr, 0 });

    SECTION("@P")
    {
        evt::node::GroupExpression::Instance inst;
        inst.group_name = "test0";
        inst.expr_str = "@P.x < 0 && @P.y < 0 && @P.z > 0";
        inst.merge_op = evt::GroupMerge::Union;
        group_expr->AddInstance(inst);

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(evt::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
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

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    box->SetSize({ 1, 2, 3 });
    box->SetCenter({ 4, 5, 6 });
    eval.AddNode(box);

    SECTION("calc")
    {
        eval.Update();

        evt::EvalContext ctx(eval, *box);
        auto sz_y = eval.CalcExpr("$SIZEY", ctx);
        REQUIRE(sz_y.type == evt::VariableType::Float);
        REQUIRE(sz_y.f == Approx(2.0f));
    }

    // fixme: prepare geo for prop
    //SECTION("prop")
    //{
    //    auto& props = const_cast<evt::NodePropsMgr&>(box->GetProps());
    //    props.SetExpr(evt::node::Box::POS_Y, "$SIZEY/2");

    //    eval.Update();

    //    test::check_aabb(box, { 3.5f, 5, 4.5f }, { 4.5f, 7, 7.5f });
    //}
}