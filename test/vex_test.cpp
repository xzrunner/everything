#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/EvalContext.h>

#include <sop/node/Box.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/Measure.h>
#include <sop/node/Grid.h>
#include <sop/node/Normal.h>
#include <sop/node/AttributeWrangle.h>

#include <catch/catch.hpp>

TEST_CASE("single quotes")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    box->SetName("box0");
    box->SetSize({ 2, 4, 6 });
    eval.AddNode(box);

    auto box2 = std::make_shared<sop::node::Box>();
    auto& parms_mgr = const_cast<sop::NodeParmsMgr&>(box2->GetParms());
    //parms_mgr.SetExpr(static_cast<int>(sop::node::Box::Parm::Size), 
    //    "`ch(\"../box0/sizex\")`", 0);
    parms_mgr.SetExpr("sizex", "`ch(\"../box0/sizex\")`");
    eval.AddNode(box2);

    eval.Connect({ box, 0 }, { box2, 0 });

    eval.Update();

    test::check_aabb(box2, { -1, -0.5f, -0.5f }, { 1, 0.5f, 0.5f });
}

TEST_CASE("fetch attr use @")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    box->SetSize({ 1, 2, 4});
    eval.AddNode(box);

    auto measure = std::make_shared<sop::node::Measure>();
    measure->SetMesureType(sop::node::Measure::Type::Perimeter);
    measure->SetMesureName("Perim");
    eval.AddNode(measure);

    eval.Connect({ box, 0 }, { measure, 0 });

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("selected");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetBaseGroupEnable(true);
    group->SetBaseGroupSyntax("@Perim<7");
    eval.AddNode(group);

    eval.Connect({ measure, 0 }, { group, 0 });

    eval.Update();

    test::check_group_num(group, "selected", 2);
}

TEST_CASE("tmp")
{
    test::init();

    sop::Evaluator eval;

    auto grid = std::make_shared<sop::node::Grid>();
    eval.AddNode(grid);

    auto normal = std::make_shared<sop::node::Normal>();
    eval.AddNode(normal);

    eval.Connect({ grid, 0 }, { normal, 0 });

    auto attr_vex = std::make_shared<sop::node::AttributeWrangle>();
    attr_vex->SetVexExpr(R"(
@Cd = float(@ptnum)/@numpt;
)");
    eval.AddNode(attr_vex);

    eval.Connect({ normal, 0 }, { attr_vex, 0 });

    eval.Update();
}