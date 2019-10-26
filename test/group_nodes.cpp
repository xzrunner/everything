#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/GeometryImpl.h>

#include <sop/node/GroupCreate.h>
#include <sop/node/GroupExpression.h>
#include <sop/node/GroupPromote.h>

#include <sop/node/Box.h>
#include <sop/node/Blast.h>
#include <sop/node/Merge.h>
#include <sop/node/PolyExtrude.h>

#include <catch/catch.hpp>

TEST_CASE("group_create")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    const sm::vec3 pos0(5, 0, 0);
    box0->SetCenter(pos0);
    eval.AddNode(box0);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("test");
    eval.AddNode(group);

    eval.Connect({ box0, 0 }, { group, 0 });

    SECTION("base group points")
    {
        group->SetGroupType(sop::GroupType::Points);
        group->EnableBaseGroup("@P.y < 0");

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            REQUIRE(test::get_point_pos(blast, i).y < 0);
        }
    }

    SECTION("base group prims")
    {
        group->SetGroupType(sop::GroupType::Primitives);
        group->EnableBaseGroup("@P.y < 0");

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        eval.AddNode(blast);

        eval.Connect({ group, 0 }, { blast, 0 });

        eval.Update();

        test::check_faces_num(blast, 5);
    }

    SECTION("base group prims 2")
    {
        group->SetGroupType(sop::GroupType::Primitives);
        group->EnableBaseGroup("@P.y < 0");

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group, 0 }, { blast, 0 });

        eval.Update();

        test::check_faces_num(blast, 1);
        test::get_face_pos(blast, 0, [](const sm::vec3& pos) {
            REQUIRE(pos.y < 0);
        });
    }

    SECTION("keey by normal")
    {
        group->SetGroupType(sop::GroupType::Primitives);
        group->EnableKeepByNormals(sm::vec3(0, 0, 1), 10);

        auto box1 = std::make_shared<sop::node::Box>();
        const sm::vec3 pos1(-5, 0, 0);
        box1->SetCenter(pos1);
        eval.AddNode(box1);

        auto merge = std::make_shared<sop::node::Merge>();
        eval.AddNode(merge);
        eval.Connect({ group, 0 }, { merge, 0 });
        eval.Connect({ box1, 0 },  { merge, 1 });

        auto extrude = std::make_shared<sop::node::PolyExtrude>();
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

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group_expr = std::make_shared<sop::node::GroupExpression>();
    group_expr->SetGroupType(sop::GroupType::Points);
    eval.AddNode(group_expr);

    eval.Connect({ box, 0 }, { group_expr, 0 });

    SECTION("replace")
    {
        sop::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str = "@P.y < 0";
        inst0.merge_op = sop::GroupMerge::Union;
        group_expr->AddInstance(inst0);

        sop::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str = "@P.x < 0";
        inst1.merge_op = sop::GroupMerge::Replace;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            auto pos = test::get_point_pos(blast, i);
            REQUIRE((pos.x < 0));
        }
    }

    SECTION("union")
    {
        sop::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str   = "@P.y < 0";
        inst0.merge_op = sop::GroupMerge::Union;
        group_expr->AddInstance(inst0);

        sop::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str   = "@P.x < 0";
        inst1.merge_op = sop::GroupMerge::Union;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 6);
        for (int i = 0; i < 6; ++i) {
            auto pos = test::get_point_pos(blast, i);
            REQUIRE((pos.y < 0 || pos.x < 0));
        }
    }

    SECTION("intersect")
    {
        sop::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str = "@P.y < 0";
        inst0.merge_op = sop::GroupMerge::Union;
        group_expr->AddInstance(inst0);

        sop::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str = "@P.x < 0";
        inst1.merge_op = sop::GroupMerge::Intersect;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 2);
        for (int i = 0; i < 2; ++i) {
            auto pos = test::get_point_pos(blast, i);
            REQUIRE((pos.y < 0 && pos.x < 0));
        }
    }

    SECTION("subtract")
    {
        sop::node::GroupExpression::Instance inst0;
        inst0.group_name = "test0";
        inst0.expr_str = "@P.y < 0";
        inst0.merge_op = sop::GroupMerge::Union;
        group_expr->AddInstance(inst0);

        sop::node::GroupExpression::Instance inst1;
        inst1.group_name = "test0";
        inst1.expr_str = "@P.x < 0";
        inst1.merge_op = sop::GroupMerge::Subtract;
        group_expr->AddInstance(inst1);

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 2);
        for (int i = 0; i < 2; ++i) {
            auto pos = test::get_point_pos(blast, i);
            REQUIRE((pos.y < 0 && pos.x >= 0));
        }
    }

    SECTION("getbbox_max")
    {
        sop::node::GroupExpression::Instance inst;
        inst.group_name = "test0";
        inst.expr_str = "@P.y==getbbox_max(0).y";
        inst.merge_op = sop::GroupMerge::Union;
        group_expr->AddInstance(inst);

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            auto pos = test::get_point_pos(blast, i);
            REQUIRE(pos.y == Approx(0.5f));
        }
    }

    SECTION("getbbox_min")
    {
        sop::node::GroupExpression::Instance inst;
        inst.group_name = "test0";
        inst.expr_str = "@P.y==getbbox_min(0).y";
        inst.merge_op = sop::GroupMerge::Union;
        group_expr->AddInstance(inst);

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("test0");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group_expr, 0 }, { blast, 0 });

        eval.Update();

        test::check_points_num(blast, 4);
        for (int i = 0; i < 4; ++i) {
            auto pos = test::get_point_pos(blast, i);
            REQUIRE(pos.y == Approx(-0.5f));
        }
    }
}

TEST_CASE("group promote")
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

    auto group_promote = std::make_shared<sop::node::GroupPromote>();
    group_promote->SetGroupName("Top");
    group_promote->SetSrcGroupType(sop::GroupType::GuessFromGroup);
    group_promote->SetDstGroupType(sop::GroupType::Points);
    eval.AddNode(group_promote);

    eval.Connect({ group, 0 }, { group_promote, 0 });

    auto group_promote2 = std::make_shared<sop::node::GroupPromote>();
    group_promote2->SetGroupName("Top");
    group_promote2->SetSrcGroupType(sop::GroupType::GuessFromGroup);
    group_promote2->SetDstGroupType(sop::GroupType::Primitives);
    eval.AddNode(group_promote2);

    eval.Connect({ group_promote, 0 }, { group_promote2, 0 });

    eval.Update();

    test::check_group_type(group_promote, "Top", sop::GroupType::Points);
    test::check_group_num(group_promote, "Top", 4);

    test::check_group_type(group_promote2, "Top", sop::GroupType::Primitives);
    test::check_group_num(group_promote2, "Top", 5);
}