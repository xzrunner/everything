#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/EvalContext.h>

#include <sop/node/Box.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/Measure.h>

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
    auto& prop_mgr = const_cast<sop::NodePropsMgr&>(box2->GetProps());
    prop_mgr.SetExpr(sop::node::Box::SIZE, "`ch(\"../box0/sizex\")`", 0);
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
    group->EnableBaseGroup("@Perim<7");
    eval.AddNode(group);

    eval.Connect({ measure, 0 }, { group, 0 });

    eval.Update();

    test::check_group_num(group, "selected", 2);
}