#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/GeometryImpl.h>
#include <sop/GeoAttrDefine.h>

#include <sop/node/Add.h>
#include <sop/node/Boolean.h>
#include <sop/node/Fuse.h>
#include <sop/node/Knife.h>
#include <sop/node/Normal.h>
#include <sop/node/PolyExtrude.h>
#include <sop/node/PolyFill.h>
#include <sop/node/PolyFrame.h>

#include <sop/node/Box.h>
#include <sop/node/Line.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/Blast.h>
#include <sop/node/Color.h>
#include <sop/node/Merge.h>
#include <sop/node/GroupExpression.h>
#include <sop/node/Grid.h>

#include <catch/catch.hpp>

// Add

TEST_CASE("add to brush")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));

    eval.Connect({ box, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 10);
}

TEST_CASE("add to brush with attr")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto color = std::make_shared<sop::node::Color>();
    color->SetAttrAddTo(sop::GeoAttrClass::Point);
    color->SetColor({ 0.3f, 0.4f, 0.5f });
    eval.AddNode(color);

    eval.Connect({ box, 0 }, { color, 0 });

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));

    eval.Connect({ color, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 10);
    test::check_attr_count(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 10);
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 6, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 7, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 8, sop::Variable(sm::vec3(0, 0, 0)));
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 9, sop::Variable(sm::vec3(0, 0, 0)));
}

TEST_CASE("add to shape")
{
    test::init();

    sop::Evaluator eval;

    auto line = std::make_shared<sop::node::Line>();
    eval.AddNode(line);

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));

    eval.Connect({ line, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 4);
}

TEST_CASE("add to shape with attr")
{
    test::init();

    sop::Evaluator eval;

    auto line = std::make_shared<sop::node::Line>();
    eval.AddNode(line);

    auto color = std::make_shared<sop::node::Color>();
    color->SetAttrAddTo(sop::GeoAttrClass::Point);
    color->SetColor({ 0.3f, 0.4f, 0.5f });
    eval.AddNode(color);

    eval.Connect({ line, 0 }, { color, 0 });

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));

    eval.Connect({ color, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 4);
    test::check_attr_count(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 4);
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 1, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0, 0, 0)));
    test::check_attr_value(add, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 3, sop::Variable(sm::vec3(0, 0, 0)));
}

TEST_CASE("add with group")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetKeepByNormals(true);
    group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDelNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto group_left = std::make_shared<sop::node::GroupCreate>();
    group_left->SetGroupName("left");
    group_left->SetGroupType(sop::GroupType::Points);
    group_left->SetBaseGroupEnable(true);
    group_left->SetBaseGroupSyntax("@P.x < 0");
    eval.AddNode(group_left);

    eval.Connect({ blast, 0 }, { group_left, 0 });

    auto blast_left = std::make_shared<sop::node::Blast>();
    blast_left->SetGroupName("left");
    blast_left->SetGroupType(sop::GroupType::GuessFromGroup);
    blast_left->SetDelNonSelected(true);
    eval.AddNode(blast_left);

    eval.Connect({ group_left, 0 }, { blast_left, 0 });

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    eval.Connect({ blast_left, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 2);
}

TEST_CASE("boolean")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    const sm::vec3 size0(4, 1, 4);
    box0->SetSize(size0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<sop::node::Box>();
    const sm::vec3 size1(1, 2, 1);
    box1->SetSize(size1);
    eval.AddNode(box1);

    auto boolean = std::make_shared<sop::node::Boolean>();
    eval.AddNode(boolean);

    eval.Connect({ box0, 0 }, { boolean, sop::node::Boolean::IDX_A });
    eval.Connect({ box1, 0 }, { boolean, sop::node::Boolean::IDX_B });

    SECTION("union")
    {
    }

    SECTION("intersect")
    {
        boolean->SetOperator(sop::node::Boolean::Operator::Intersect);

        eval.Update();

        test::check_points_num(boolean, 8);
        test::check_faces_num(boolean, 6);
        test::check_halfedge_edges_num(boolean, 24);

        test::check_aabb(boolean, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("subtract")
    {
        boolean->SetOperator(sop::node::Boolean::Operator::Subtract);

        eval.Update();

        //test::check_points_num(boolean, 8);
        //test::check_faces_num(boolean, 6);
        //test::check_halfedge_edges_num(boolean, 24);

        test::check_aabb(boolean, sm::vec3(-2, -0.5f, -2), sm::vec3(2, 0.5f, 2));
    }
}

TEST_CASE("boolean 2")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    const sm::vec3 size0(4, 1, 4);
    box0->SetSize(size0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<sop::node::Box>();
    const sm::vec3 size1(6, 6, 6);
    box1->SetSize(size1);
    box1->SetCenter(sm::vec3(3, 0, 0));
    eval.AddNode(box1);

    auto boolean = std::make_shared<sop::node::Boolean>();
    eval.AddNode(boolean);

    eval.Connect({ box0, 0 }, { boolean, sop::node::Boolean::IDX_A });
    eval.Connect({ box1, 0 }, { boolean, sop::node::Boolean::IDX_B });

    SECTION("union")
    {
    }

    SECTION("intersect")
    {
        boolean->SetOperator(sop::node::Boolean::Operator::Intersect);

        eval.Update();

        test::check_points_num(boolean, 8);
        test::check_faces_num(boolean, 6);
        test::check_halfedge_edges_num(boolean, 24);

        test::check_aabb(boolean, sm::vec3(0, -0.5f, -2), sm::vec3(2, 0.5f, 2));
    }

    SECTION("subtract")
    {
        boolean->SetOperator(sop::node::Boolean::Operator::Subtract);

        eval.Update();

        test::check_points_num(boolean, 8);
        test::check_faces_num(boolean, 6);
        test::check_halfedge_edges_num(boolean, 24);

        test::check_aabb(boolean, sm::vec3(-2, -0.5f, -2), sm::vec3(0, 0.5f, 2));
    }
}

TEST_CASE("boolean surface")
{
    test::init();

    sop::Evaluator eval;

    auto grid = std::make_shared<sop::node::Grid>();
    grid->SetSize({ 4, 4 });
    grid->SetRows(2);
    grid->SetColumns(2);
    eval.AddNode(grid);

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    eval.Connect({ grid, 0 }, { box, 0 });

    auto boolean = std::make_shared<sop::node::Boolean>();
    eval.AddNode(boolean);

    eval.Connect({ grid, 0 }, { boolean, sop::node::Boolean::IDX_A });
    eval.Connect({ box,  0 }, { boolean, sop::node::Boolean::IDX_B });

    //SECTION("solid - solid")
    //{
    //    boolean->SetTreatAs(sop::node::Boolean::TreatAs::Solid, sop::node::Boolean::TreatAs::Solid);
    //    boolean->SetOperator(sop::node::Boolean::Operator::Subtract);

    //    eval.Update();

    //    test::check_points_num(boolean, 12);
    //    test::check_aabb(boolean, { -2, -0.5f, -2 }, { 2, 0, 2 });
    //}

    //SECTION("surface - solid")
    //{
    //    boolean->SetTreatAs(sop::node::Boolean::TreatAs::Surface, sop::node::Boolean::TreatAs::Solid);

    //    eval.Update();
    //}
}

TEST_CASE("fuse brush")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    eval.AddNode(box0);

    auto group0 = std::make_shared<sop::node::GroupCreate>();
    group0->SetGroupName("top");
    group0->SetGroupType(sop::GroupType::Primitives);
    group0->SetKeepByNormals(true);
    group0->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group0->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group0);

    eval.Connect({ box0, 0 }, { group0, 0 });

    auto blast0 = std::make_shared<sop::node::Blast>();
    blast0->SetGroupName("top");
    blast0->SetGroupType(sop::GroupType::GuessFromGroup);
    eval.AddNode(blast0);

    eval.Connect({ group0, 0 }, { blast0, 0 });

    auto box1 = std::make_shared<sop::node::Box>();
    box1->SetCenter({ 0, 1, 0 });
    eval.AddNode(box1);

    auto group1 = std::make_shared<sop::node::GroupCreate>();
    group1->SetGroupName("btm");
    group1->SetGroupType(sop::GroupType::Primitives);
    group1->SetKeepByNormals(true);
    group1->SetKeepByNormalsDir(sm::vec3(0, -1, 0));
    group1->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group1);

    eval.Connect({ box1, 0 }, { group1, 0 });

    auto blast1 = std::make_shared<sop::node::Blast>();
    blast1->SetGroupName("btm");
    blast1->SetGroupType(sop::GroupType::GuessFromGroup);
    eval.AddNode(blast1);

    eval.Connect({ group1, 0 }, { blast1, 0 });

    auto merge = std::make_shared<sop::node::Merge>();
    eval.AddNode(merge);

    eval.Connect({ blast0, 0 }, { merge, sop::node::Merge::IDX_SRC_A });
    eval.Connect({ blast1, 0 }, { merge, sop::node::Merge::IDX_SRC_B });

    auto fuse = std::make_shared<sop::node::Fuse>();
    eval.AddNode(fuse);

    eval.Connect({ merge, 0 }, { fuse, 0 });

    eval.Update();

    test::check_points_num(merge, 16);
    test::check_vertices_num(merge, 40);
    test::check_faces_num(merge, 10);
    test::check_halfedge_vertices_num(merge, 16);
    test::check_halfedge_edges_num(merge, 40);
    test::check_halfedge_faces_num(merge, 10);

    test::check_points_num(fuse, 12);
    test::check_vertices_num(fuse, 40);
    test::check_faces_num(fuse, 10);
    test::check_halfedge_vertices_num(fuse, 12);
    test::check_halfedge_edges_num(fuse, 40);
    test::check_halfedge_faces_num(fuse, 10);
}

TEST_CASE("fuse polyline")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    const sm::vec3 p(0, 1, 0);
    add->SetPoints({ p, p });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));
    eval.AddNode(add);

    auto color = std::make_shared<sop::node::Color>();
    color->SetColor(sm::vec3(1, 0, 0));
    eval.AddNode(color);

    eval.Connect({ add, 0 }, { color, 0 });

    auto fuse = std::make_shared<sop::node::Fuse>();
    eval.AddNode(fuse);

    eval.Connect({ color, 0 }, { fuse, 0 });

    eval.Update();

    test::check_points_num(fuse, 1);
    test::check_point(fuse, 0, p);
    test::check_attr_value(fuse, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(1, 0, 0)));
}

TEST_CASE("fuse unique points")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetKeepByNormals(true);
    group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDelNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto knife0 = std::make_shared<sop::node::Knife>();
    knife0->SetDirection({ 1, 0, 0 });
    eval.AddNode(knife0);

    eval.Connect({ blast, 0 }, { knife0, 0 });

    auto knife1 = std::make_shared<sop::node::Knife>();
    knife1->SetDirection({ 0, 0, 1 });
    eval.AddNode(knife1);

    eval.Connect({ knife0, 0 }, { knife1, 0 });

    auto fuse = std::make_shared<sop::node::Fuse>();
    fuse->SetFuseOP(sop::node::Fuse::Operator::UniquePoints);
    eval.AddNode(fuse);

    eval.Connect({ knife1, 0 }, { fuse, 0 });

    eval.Update();

    test::check_vertices_num(knife1, 16);
    test::check_points_num(knife1, 9);
    test::check_vertices_num(fuse, 16);
    test::check_points_num(fuse, 16);
}

TEST_CASE("fuse polyline2")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    const sm::vec3 p(0, 1, 0);
    add->SetPoints({
        { 0, 1, 0 },
        { 0, 2, 0 },
        { 0, 2, 0 },
        { 0, 3, 0 },
        { 0, 4, 0 },
        { 0, 4, 0 },
        { 0, 4, 0 },
    });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));
    eval.AddNode(add);

    auto color = std::make_shared<sop::node::Color>();
    color->SetColor(sm::vec3(1, 0, 0));
    eval.AddNode(color);

    eval.Connect({ add, 0 }, { color, 0 });

    auto fuse = std::make_shared<sop::node::Fuse>();
    eval.AddNode(fuse);

    eval.Connect({ color, 0 }, { fuse, 0 });

    eval.Update();

    test::check_points_num(fuse, 4);
    test::check_point(fuse, 0, p);
    test::check_attr_value(fuse, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(1, 0, 0)));
}

TEST_CASE("knife box")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto knife = std::make_shared<sop::node::Knife>();
    eval.AddNode(knife);

    eval.Connect({ box, 0 }, { knife, 0 });

    SECTION("keep all")
    {
        knife->SetKeepType(sop::node::Knife::KeepType::KeepAll);

        eval.Update();

        test::check_points_num(knife, 12);
        test::check_faces_num(knife, 10);
        test::check_halfedge_edges_num(knife, 40);

        test::check_aabb(knife, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep above")
    {
        knife->SetKeepType(sop::node::Knife::KeepType::KeepAbove);

        eval.Update();

        test::check_points_num(knife, 8);
        test::check_faces_num(knife, 5);
        test::check_halfedge_edges_num(knife, 20);

        test::check_aabb(knife, sm::vec3(-0.5f, 0, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep below")
    {
        knife->SetKeepType(sop::node::Knife::KeepType::KeepBelow);

        eval.Update();

        test::check_points_num(knife, 8);
        test::check_faces_num(knife, 5);
        test::check_halfedge_edges_num(knife, 20);

        test::check_aabb(knife, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0, 0.5f));
    }
}

TEST_CASE("knife plane")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetKeepByNormals(true);
    group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDelNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto knife = std::make_shared<sop::node::Knife>();
    eval.AddNode(knife);

    eval.Connect({ blast, 0 }, { knife, 0 });

    SECTION("dir x")
    {
        knife->SetKeepType(sop::node::Knife::KeepType::KeepAll);
        knife->SetDirection({ 1, 0, 0 });

        eval.Update();

        test::check_faces_num(knife, 2);
        test::check_points_num(knife, 6);
        test::check_halfedge_edges_num(knife, 8);

        test::check_group_num(blast, "top", 1);
        test::check_group_num(knife, "top", 2);
    }

    SECTION("dir neg x")
    {
        knife->SetKeepType(sop::node::Knife::KeepType::KeepAll);
        knife->SetDirection({ -1, 0, 0 });

        eval.Update();

        test::check_faces_num(knife, 2);
        test::check_points_num(knife, 6);
        test::check_halfedge_edges_num(knife, 8);

        test::check_group_num(blast, "top", 1);
        test::check_group_num(knife, "top", 2);
    }

    SECTION("knife 2")
    {
        knife->SetKeepType(sop::node::Knife::KeepType::KeepAll);
        knife->SetDirection({ 1, 0, 0 });

        auto knife2 = std::make_shared<sop::node::Knife>();
        knife2->SetKeepType(sop::node::Knife::KeepType::KeepAll);
        knife2->SetDirection({ 0, 0, 1 });
        eval.AddNode(knife2);

        eval.Connect({ knife, 0 }, { knife2, 0 });

        eval.Update();

        test::check_faces_num(knife2, 4);
        test::check_group_num(knife2, "top", 4);
    }
}

TEST_CASE("normal box")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto normal = std::make_shared<sop::node::Normal>();
    eval.AddNode(normal);

    eval.Connect({ box, 0 }, { normal, 0 });

    SECTION("add to point")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Points);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 8);
    }

    SECTION("add to vertex")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Vertices);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 24);
    }

    SECTION("add to primitive")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Primitives);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 6);
    }

    SECTION("add to detail")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Detail);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1);
        test::check_attr_value(normal, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 0, 0)));
    }
}

TEST_CASE("normal plane")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("Top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetKeepByNormals(true);
    group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("Top");
    blast->SetGroupType(sop::GroupType::Primitives);
    blast->SetDelNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto normal = std::make_shared<sop::node::Normal>();
    eval.AddNode(normal);

    eval.Connect({ blast, 0 }, { normal, 0 });

    SECTION("add to point")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Points);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 4);
        for (int i = 0; i < 4; ++i) {
            test::check_attr_value(normal, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], i, sop::Variable(sm::vec3(0, 1, 0)));
        }
    }

    SECTION("add to vertex")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Vertices);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 4);
        for (int i = 0; i < 4; ++i) {
            test::check_attr_value(normal, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_NORM], i, sop::Variable(sm::vec3(0, 1, 0)));
        }
    }

    SECTION("add to primitive")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Primitives);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1);
        test::check_attr_value(normal, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 1, 0)));
    }

    SECTION("add to detail")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Detail);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1);
        test::check_attr_value(normal, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 1, 0)));
    }
}

TEST_CASE("normal line")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    const int n = 3;
    add->SetPoints({
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
    });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));
    eval.AddNode(add);

    auto normal = std::make_shared<sop::node::Normal>();
    eval.AddNode(normal);

    eval.Connect({ add, 0 }, { normal, 0 });

    SECTION("add to point")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Points);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], n);
        for (int i = 0; i < n; ++i) {
            test::check_attr_value(normal, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_NORM], i, sop::Variable(sm::vec3(0, 0, 0)));
        }
    }

    SECTION("add to vertex")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Vertices);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_NORM], n);
        for (int i = 0; i < n; ++i) {
            test::check_attr_value(normal, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_NORM], i, sop::Variable(sm::vec3(0, 0, 0)));
        }
    }

    SECTION("add to primitive")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Primitives);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1);
        test::check_attr_value(normal, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 0, 0)));
    }

    SECTION("add to detail")
    {
        normal->SetAttrAddTo(sop::node::Normal::AddToType::Detail);

        eval.Update();

        test::check_attr_count(normal, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1);
        test::check_attr_value(normal, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0 ,0, 0)));
    }
}

TEST_CASE("poly extrude")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    const sm::vec3 size(1, 2, 3);
    box->SetSize(size);
    eval.AddNode(box);

    SECTION("extrude the whole box")
    {
        auto extrude = std::make_shared<sop::node::PolyExtrude>();
        const float dist = 1.1f;
        extrude->SetDistance(dist);
        eval.AddNode(extrude);

        eval.Connect({ box, 0 }, { extrude, 0 });

        eval.Update();

        auto h_sz = size * 0.5f;
        test::check_aabb(extrude, -h_sz - sm::vec3(dist, dist, dist), h_sz + sm::vec3(dist, dist, dist));
    }

    SECTION("extrude one face")
    {
        auto group = std::make_shared<sop::node::GroupCreate>();
        const std::string name("test");
        group->SetGroupName(name);
        group->SetGroupType(sop::GroupType::Primitives);
        group->SetKeepByNormals(true);
        group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
        group->SetKeepByNormalsAngle(10.0f);
        eval.AddNode(group);

        eval.Connect({ box, 0 }, { group, 0 });

        auto extrude = std::make_shared<sop::node::PolyExtrude>();
        extrude->SetGroupName(name);
        const float dist = 1.1f;
        extrude->SetDistance(dist);
        eval.AddNode(extrude);

        eval.Connect({ group, 0 }, { extrude, 0 });

        eval.Update();

        auto h_sz = size * 0.5f;
        test::check_aabb(extrude, -h_sz, sm::vec3(h_sz.x, h_sz.y + dist, h_sz.z));
    }
}

TEST_CASE("poly extrude face")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetKeepByNormals(true);
    group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDelNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto extrude = std::make_shared<sop::node::PolyExtrude>();
    extrude->SetDistance(0.5f);
    extrude->SetOutputFrontGroup(true);
    extrude->SetFrontGroupName("new_front_faces");
    extrude->SetOutputSideGroup(true);
    extrude->SetSideGroupName("new_side_faces");
    eval.AddNode(extrude);

    eval.Connect({ blast, 0 }, { extrude, 0 });

    eval.Update();

    test::check_faces_num(extrude, 5);
    test::check_aabb(extrude, { -0.5f, 0.5f, -0.5f }, { 0.5f, 1, 0.5f });

    test::check_group_type(extrude, "new_front_faces", sop::GroupType::Primitives);
    test::check_group_num(extrude, "new_front_faces", 1);
    test::check_group_num(extrude, "new_side_faces", 4);
}

TEST_CASE("poly fill")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetKeepByNormals(true);
    group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    SECTION("one face")
    {
        auto poly_fill = std::make_shared<sop::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ blast, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_faces_num(blast, 5);
        test::check_faces_num(poly_fill, 6);
    }

    SECTION("two face")
    {
        auto group2 = std::make_shared<sop::node::GroupCreate>();
        group2->SetGroupName("btm");
        group2->SetGroupType(sop::GroupType::Primitives);
        group2->SetKeepByNormals(true);
        group2->SetKeepByNormalsDir(sm::vec3(0, -1, 0));
        group2->SetKeepByNormalsAngle(10.0f);
        eval.AddNode(group2);

        eval.Connect({ blast, 0 }, { group2, 0 });

        auto blast2 = std::make_shared<sop::node::Blast>();
        blast2->SetGroupName("btm");
        blast2->SetGroupType(sop::GroupType::GuessFromGroup);
        eval.AddNode(blast2);

        eval.Connect({ group2, 0 }, { blast2, 0 });

        auto poly_fill = std::make_shared<sop::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ blast2, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_faces_num(blast, 5);
        test::check_faces_num(blast2, 4);
        test::check_faces_num(poly_fill, 6);
    }

    SECTION("two box")
    {
        auto box2 = std::make_shared<sop::node::Box>();
        box2->SetCenter({ 0, 1, 0 });
        eval.AddNode(box2);

        auto group2 = std::make_shared<sop::node::GroupCreate>();
        group2->SetGroupName("btm");
        group2->SetGroupType(sop::GroupType::Primitives);
        group2->SetKeepByNormals(true);
        group2->SetKeepByNormalsDir(sm::vec3(0, -1, 0));
        group2->SetKeepByNormalsAngle(10.0f);
        eval.AddNode(group2);

        eval.Connect({ box2, 0 }, { group2, 0 });

        auto blast2 = std::make_shared<sop::node::Blast>();
        blast2->SetGroupName("btm");
        blast2->SetGroupType(sop::GroupType::GuessFromGroup);
        eval.AddNode(blast2);

        eval.Connect({ group2, 0 }, { blast2, 0 });

        auto merge = std::make_shared<sop::node::Merge>();
        eval.AddNode(merge);

        eval.Connect({ blast, 0 },  { merge, sop::node::Merge::IDX_SRC_A });
        eval.Connect({ blast2, 0 }, { merge, sop::node::Merge::IDX_SRC_B });

        auto poly_fill = std::make_shared<sop::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ merge, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_faces_num(merge, 10);
        test::check_halfedge_faces_num(merge, 10);
        test::check_halfedge_edges_num(merge, 40);

        test::check_faces_num(poly_fill, 12);
        test::check_halfedge_faces_num(poly_fill, 12);
        test::check_halfedge_edges_num(poly_fill, 48);
    }

    SECTION("point attr rebuild")
    {
        auto color = std::make_shared<sop::node::Color>();
        color->SetAttrAddTo(sop::GeoAttrClass::Point);
        color->SetColor({ 0.3f, 0.4f, 0.5f });
        eval.AddNode(color);

        eval.Connect({ blast, 0 }, { color, 0 });

        auto poly_fill = std::make_shared<sop::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ color, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_attr_count(poly_fill, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 8);
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 5, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 6, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    }

    SECTION("vertex attr rebuild")
    {
        auto color = std::make_shared<sop::node::Color>();
        color->SetAttrAddTo(sop::GeoAttrClass::Vertex);
        color->SetColor({ 0.3f, 0.4f, 0.5f });
        eval.AddNode(color);

        eval.Connect({ blast, 0 }, { color, 0 });

        auto poly_fill = std::make_shared<sop::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ color, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_attr_count(poly_fill, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_CD], 24);
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_CD], 5, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Vertex, sop::GeoAttrNames[sop::GEO_ATTR_CD], 6, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    }

    SECTION("primitive attr rebuild")
    {
        auto color = std::make_shared<sop::node::Color>();
        color->SetAttrAddTo(sop::GeoAttrClass::Primitive);
        color->SetColor({ 0.3f, 0.4f, 0.5f });
        eval.AddNode(color);

        eval.Connect({ blast, 0 }, { color, 0 });

        auto poly_fill = std::make_shared<sop::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ color, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_attr_count(poly_fill, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_CD], 6);
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_CD], 1, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_CD], 3, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_CD], 4, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_CD], 5, sop::Variable(sm::vec3(0, 0, 0)));
    }

    SECTION("detail attr rebuild")
    {
        auto color = std::make_shared<sop::node::Color>();
        color->SetAttrAddTo(sop::GeoAttrClass::Detail);
        color->SetColor({ 0.3f, 0.4f, 0.5f });
        eval.AddNode(color);

        eval.Connect({ blast, 0 }, { color, 0 });

        auto poly_fill = std::make_shared<sop::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ color, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_attr_count(poly_fill, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0);
        test::check_attr_value(poly_fill, sop::GeoAttrClass::Detail, sop::GeoAttrNames[sop::GEO_ATTR_CD], 0, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    }
}

TEST_CASE("poly frame shape(2 pts)")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({ { 0, 0, 0 }, { 1, 2, 3 } });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));
    eval.AddNode(add);

    auto polyframe = std::make_shared<sop::node::PolyFrame>();
    polyframe->SetEntityType(sop::GroupType::Primitives);
    polyframe->SetFrameStyle(sop::node::PolyFrame::FrameStyle::TwoEdges);
    polyframe->SetNormalNameToggle(true);
    polyframe->SetTangentNameToggle(true);
    polyframe->SetBitangentNameToggle(true);
    polyframe->SetNormalName("N");
    polyframe->SetTangentName("T");
    polyframe->SetBitangentName("B");
    eval.AddNode(polyframe);

    eval.Connect({ add, 0 }, { polyframe, 0 });

    eval.Update();

    test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "N", 2);
    test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "T", 2);
    test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "B", 2);

    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 0, sop::Variable(sm::vec3(0, 0, 0)));
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 1, sop::Variable(sm::vec3(0, 0, 0)));
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 0, sop::Variable(sm::vec3(-0.267261f, -0.534522f, -0.801784f)), true);
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 1, sop::Variable(sm::vec3(-0.267261f, -0.534522f, -0.801784f)), true);
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 0, sop::Variable(sm::vec3(0, 0, 0)));
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 1, sop::Variable(sm::vec3(0, 0, 0)));
}

TEST_CASE("poly frame shape - norm attr (2 pts)")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    box->SetSize({ 1, 2, 4 });
    eval.AddNode(box);

    auto normal = std::make_shared<sop::node::Normal>();
    normal->SetAttrAddTo(sop::node::Normal::AddToType::Points);
    eval.AddNode(normal);

    eval.Connect({ box, 0 }, { normal, 0 });

    auto group_expr = std::make_shared<sop::node::GroupExpression>();
    group_expr->SetGroupType(sop::GroupType::Points);
    group_expr->SetGroupNames({ "group1" });
    group_expr->SetGroupExprs({ "@P.x < 0 && @P.y > 0" });
    group_expr->SetGroupMergeOps({ sop::GroupMerge::Replace });
    eval.AddNode(group_expr);

    eval.Connect({ normal, 0 }, { group_expr, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("group1");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDelNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group_expr, 0 }, { blast, 0 });

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    eval.Connect({ blast, 0 }, { add, 0 });

    auto polyframe = std::make_shared<sop::node::PolyFrame>();
    polyframe->SetEntityType(sop::GroupType::Primitives);
    polyframe->SetFrameStyle(sop::node::PolyFrame::FrameStyle::TwoEdges);
    polyframe->SetNormalNameToggle(true);
    polyframe->SetTangentNameToggle(true);
    polyframe->SetBitangentNameToggle(true);
    polyframe->SetNormalName("N");
    polyframe->SetTangentName("T");
    polyframe->SetBitangentName("B");
    eval.AddNode(polyframe);

    eval.Connect({ add, 0 }, { polyframe, 0 });

    eval.Update();

    test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "N", 2);
    test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "T", 2);
    test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "B", 2);

    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 0, sop::Variable(sm::vec3(-0.57735f, 0.57735f, -0.57735f)), true);
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 1, sop::Variable(sm::vec3(-0.57735f, 0.57735f, 0.57735f)), true);
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 0, sop::Variable(sm::vec3(0, 0, -1)));
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 1, sop::Variable(sm::vec3(0, 0, -1)));
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 0, sop::Variable(sm::vec3(0.57735f, 0.57735f, 0)), true);
    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 1, sop::Variable(sm::vec3(0.57735f, 0.57735f, 0)), true);
}

TEST_CASE("poly frame shape - calc norm (3 pts)")
{
    test::init();

    sop::Evaluator eval;

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({ { 0, 0, 0 }, { 1, 1, 1 }, { 0, 0, 2 } });
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));
    eval.AddNode(add);

    auto polyframe = std::make_shared<sop::node::PolyFrame>();
    polyframe->SetEntityType(sop::GroupType::Primitives);
    polyframe->SetNormalNameToggle(true);
    polyframe->SetTangentNameToggle(true);
    polyframe->SetBitangentNameToggle(true);
    polyframe->SetNormalName("N");
    polyframe->SetTangentName("T");
    polyframe->SetBitangentName("B");
    eval.AddNode(polyframe);

    eval.Connect({ add, 0 }, { polyframe, 0 });

    SECTION("style first edge")
    {
        polyframe->SetFrameStyle(sop::node::PolyFrame::FrameStyle::FirstEdge);

        eval.Update();

        test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "N", 3);
        test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "T", 3);
        test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "B", 3);

        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 0, sop::Variable(sm::vec3(-0.707107f, 0.707107f, 0.0f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 1, sop::Variable(sm::vec3(-0.707107f, 0.707107f, 0.0f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 2, sop::Variable(sm::vec3(-0.707107f, 0.707107f, 0.0f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 0, sop::Variable(sm::vec3(-0.57735f, -0.57735f, -0.57735f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 1, sop::Variable(sm::vec3(-0.57735f, -0.57735f, -0.57735f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 2, sop::Variable(sm::vec3(0.57735f, 0.57735f, -0.57735f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 0, sop::Variable(sm::vec3(0.408248f, 0.408248f, -0.816497f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 1, sop::Variable(sm::vec3(0.408248f, 0.408248f, -0.816497f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 2, sop::Variable(sm::vec3(0.408248f, 0.408248f, 0.816497f)), true);
    }

    SECTION("style two edges")
    {
        polyframe->SetFrameStyle(sop::node::PolyFrame::FrameStyle::TwoEdges);

        eval.Update();

        test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "N", 3);
        test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "T", 3);
        test::check_attr_count(polyframe, sop::GeoAttrClass::Point, "B", 3);

        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 0, sop::Variable(sm::vec3(-0.707107f, 0.707107f, 0.0f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 1, sop::Variable(sm::vec3(-0.707107f, 0.707107f, 0.0f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 2, sop::Variable(sm::vec3(-0.707107f, 0.707107f, 0.0f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 0, sop::Variable(sm::vec3(-0.57735f, -0.57735f, -0.57735f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 1, sop::Variable(sm::vec3(0, 0, -1)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "T", 2, sop::Variable(sm::vec3(0.57735f, 0.57735f, -0.57735f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 0, sop::Variable(sm::vec3(0.408248f, 0.408248f, -0.816497f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 1, sop::Variable(sm::vec3(0.707107f, 0.707107f, 0.0f)), true);
        test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "B", 2, sop::Variable(sm::vec3(0.408248f, 0.408248f, 0.816497f)), true);

    }
}

//TEST_CASE("poly frame shape smooth(4 pts)")
//{
//    test::init();
//
//    sop::Evaluator eval;
//
//    auto add = std::make_shared<sop::node::Add>();
//    add->SetPoints({ { 1.001f, 3, -20 }, { 0, 0, 0 }, { 1, 3, 1 }, { 0, 0, 1 } });
//    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));
//    eval.AddNode(add);
//
//    auto polyframe = std::make_shared<sop::node::PolyFrame>();
//    polyframe->SetEntityType(sop::GroupType::Primitives);
//    polyframe->SetNormalNameToggle(true);
//    polyframe->SetTangentNameToggle(true);
//    polyframe->SetBitangentNameToggle(true);
//    polyframe->SetNormalName("N");
//    polyframe->SetTangentName("T");
//    polyframe->SetBitangentName("B");
//    eval.AddNode(polyframe);
//
//    eval.Connect({ add, 0 }, { polyframe, 0 });
//
//
//    polyframe->SetFrameStyle(sop::node::PolyFrame::FrameStyle::FirstEdge);
//
//    eval.Update();
//
//    // todo: how polyframe calc smoothed curve normal ?
//    test::check_attr_value(polyframe, sop::GeoAttrClass::Point, "N", 0, sop::Variable(sm::vec3(0, -1, 0)));
//}