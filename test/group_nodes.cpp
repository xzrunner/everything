#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>

#include <everything/node/GroupCreate.h>
#include <everything/node/GroupExpression.h>

#include <everything/node/Box.h>
#include <everything/node/Blast.h>
#include <everything/node/Merge.h>
#include <everything/node/PolyExtrude.h>

#include <catch/catch.hpp>

TEST_CASE("group_create")
{
    test::init();

    evt::Evaluator eval;

    auto box0 = std::make_shared<evt::node::Box>();
    const sm::vec3 pos0(5, 0, 0);
    box0->SetCenter(pos0);
    eval.AddNode(box0);

    auto group = std::make_shared<evt::node::GroupCreate>();
    group->SetName("test");
    eval.AddNode(group);

    eval.Connect({ box0, 0 }, { group, 0 });

    SECTION("base group")
    {
        group->SetType(evt::GroupType::Points);
        group->EnableBaseGroup("@P.y < 0");

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test");
        blast->SetGroupType(evt::GroupType::Points);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            REQUIRE(test::get_pos(blast, i).y < 0);
        }
    }

    SECTION("keey by normal")
    {
        group->SetType(evt::GroupType::Primitives);
        group->EnableKeepByNormals(sm::vec3(0, 0, 1), 10);

        auto box1 = std::make_shared<evt::node::Box>();
        const sm::vec3 pos1(-5, 0, 0);
        box1->SetCenter(pos1);
        eval.AddNode(box1);

        auto merge = std::make_shared<evt::node::Merge>();
        eval.AddNode(merge);
        eval.Connect({ group, 0 }, { merge, 0 });
        eval.Connect({ box1, 0 },  { merge, 1 });

        auto extrude = std::make_shared<evt::node::PolyExtrude>();
        extrude->SetGroupName("test");
        extrude->SetDistance(5);
        eval.AddNode(extrude);
        eval.Connect({ merge, 0 }, { extrude, 0 });

        eval.Update();

        test::check_points_num(merge, 16);
        test::check_faces_num(merge, 12);

        test::check_aabb(extrude, sm::vec3(-5.5f, -0.5f, -0.5f), sm::vec3(5.5f, 0.5f, 5.5f));
    }
}

TEST_CASE("group_expression")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto group_expr = std::make_shared<evt::node::GroupExpression>();
    group_expr->SetType(evt::GroupType::Points);
    eval.AddNode(group_expr);

    eval.Connect({ box, 0 }, { group_expr, 0 });

    SECTION("replace")
    {
        evt::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str = "@P.y < 0";
        inst0.merge_op = evt::node::GroupExpression::MergeOP::Union;
        group_expr->AddInstance(inst0);

        evt::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str = "@P.x < 0";
        inst1.merge_op = evt::node::GroupExpression::MergeOP::Replace;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(evt::GroupType::Points);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            auto pos = test::get_pos(blast, i);
            REQUIRE((pos.x < 0));
        }
    }

    SECTION("union")
    {
        evt::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str   = "@P.y < 0";
        inst0.merge_op = evt::node::GroupExpression::MergeOP::Union;
        group_expr->AddInstance(inst0);

        evt::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str   = "@P.x < 0";
        inst1.merge_op = evt::node::GroupExpression::MergeOP::Union;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(evt::GroupType::Points);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 6);
        for (int i = 0; i < 6; ++i) {
            auto pos = test::get_pos(blast, i);
            REQUIRE((pos.y < 0 || pos.x < 0));
        }
    }

    SECTION("intersect")
    {
        evt::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str = "@P.y < 0";
        inst0.merge_op = evt::node::GroupExpression::MergeOP::Union;
        group_expr->AddInstance(inst0);

        evt::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str = "@P.x < 0";
        inst1.merge_op = evt::node::GroupExpression::MergeOP::Intersect;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(evt::GroupType::Points);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 2);
        for (int i = 0; i < 2; ++i) {
            auto pos = test::get_pos(blast, i);
            REQUIRE((pos.y < 0 && pos.x < 0));
        }
    }

    SECTION("subtract")
    {
        evt::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str = "@P.y < 0";
        inst0.merge_op = evt::node::GroupExpression::MergeOP::Union;
        group_expr->AddInstance(inst0);

        evt::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str = "@P.x < 0";
        inst1.merge_op = evt::node::GroupExpression::MergeOP::Subtract;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(evt::GroupType::Points);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 2);
        for (int i = 0; i < 2; ++i) {
            auto pos = test::get_pos(blast, i);
            REQUIRE((pos.y < 0 && pos.x >= 0));
        }
    }

    SECTION("getbbox_max")
    {
        evt::node::GroupExpression::Instance inst;
        inst.group_name = "test0";
        inst.expr_str = "@P.y==getbbox_max(0).y";
        inst.merge_op = evt::node::GroupExpression::MergeOP::Union;
        group_expr->AddInstance(inst);

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(evt::GroupType::Points);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            auto pos = test::get_pos(blast, i);
            REQUIRE(pos.y == Approx(0.5f));
        }
    }

    SECTION("getbbox_min")
    {
        evt::node::GroupExpression::Instance inst;
        inst.group_name = "test0";
        inst.expr_str = "@P.y==getbbox_min(0).y";
        inst.merge_op = evt::node::GroupExpression::MergeOP::Union;
        group_expr->AddInstance(inst);

        auto blast = std::make_shared<evt::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(evt::GroupType::Points);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            auto pos = test::get_pos(blast, i);
            REQUIRE(pos.y == Approx(-0.5f));
        }
    }
}