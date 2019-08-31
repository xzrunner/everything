#include "utility.h"

#include <everything/TreeContext.h>
#include <everything/Evaluator.h>
#include <everything/EvalContext.h>

#include <everything/node/Box.h>
#include <everything/node/Geometry.h>

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
