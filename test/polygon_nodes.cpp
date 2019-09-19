#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>
#include <everything/GeoAttrName.h>

#include <everything/node/Add.h>
#include <everything/node/Boolean.h>
#include <everything/node/Knife.h>
#include <everything/node/Normal.h>
#include <everything/node/PolyExtrude.h>
#include <everything/node/PolyFill.h>

#include <everything/node/Box.h>
#include <everything/node/Line.h>
#include <everything/node/GroupCreate.h>
#include <everything/node/Blast.h>
#include <everything/node/Color.h>

#include <catch/catch.hpp>

// Add

TEST_CASE("add to brush")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto add = std::make_shared<evt::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });

    eval.Connect({ box, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 10);
}

TEST_CASE("add to brush with attr")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto color = std::make_shared<evt::node::Color>();
    color->SetAttrAddTo(evt::GeoAttrType::Point);
    color->SetColor({ 0.3f, 0.4f, 0.5f });
    eval.AddNode(color);

    eval.Connect({ box, 0 }, { color, 0 });

    auto add = std::make_shared<evt::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });

    eval.Connect({ color, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 10);
    test::check_attr_count(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 10);
    test::check_attr_count(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 10);
    test::check_attr_count(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 10);
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 6, evt::Variable(0.3f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 8, evt::Variable(0.0f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.4f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 9, evt::Variable(0.0f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 7, evt::Variable(0.5f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 8, evt::Variable(0.0f));
}

TEST_CASE("add to shape")
{
    test::init();

    evt::Evaluator eval;

    auto line = std::make_shared<evt::node::Line>();
    eval.AddNode(line);

    auto add = std::make_shared<evt::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });

    eval.Connect({ line, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 4);
}

TEST_CASE("add to shape with attr")
{
    test::init();

    evt::Evaluator eval;

    auto line = std::make_shared<evt::node::Line>();
    eval.AddNode(line);

    auto color = std::make_shared<evt::node::Color>();
    color->SetAttrAddTo(evt::GeoAttrType::Point);
    color->SetColor({ 0.3f, 0.4f, 0.5f });
    eval.AddNode(color);

    eval.Connect({ line, 0 }, { color, 0 });

    auto add = std::make_shared<evt::node::Add>();
    eval.AddNode(add);

    add->SetPoints({ {1, 2, 3}, {4, 5, 6} });

    eval.Connect({ color, 0 }, { add, 0 });

    eval.Update();

    test::check_points_num(add, 4);
    test::check_attr_count(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 4);
    test::check_attr_count(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 4);
    test::check_attr_count(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 4);
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.3f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.0f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 3, evt::Variable(0.0f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.5f));
    test::check_attr_value(add, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.0f));
}

TEST_CASE("boolean")
{
    test::init();

    evt::Evaluator eval;

    auto box0 = std::make_shared<evt::node::Box>();
    const sm::vec3 size0(4, 1, 4);
    box0->SetSize(size0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<evt::node::Box>();
    const sm::vec3 size1(1, 2, 1);
    box1->SetSize(size1);
    eval.AddNode(box1);

    auto boolean = std::make_shared<evt::node::Boolean>();
    eval.AddNode(boolean);

    eval.Connect({ box0, 0 }, { boolean, evt::node::Boolean::IDX_A });
    eval.Connect({ box1, 0 }, { boolean, evt::node::Boolean::IDX_B });

    SECTION("union")
    {
    }

    SECTION("intersect")
    {
        boolean->SetOperator(evt::node::Boolean::Operator::Intersect);

        eval.Update();

        test::check_points_num(boolean, 8);
        //test::check_edges_num(boolean, 24);
        test::check_faces_num(boolean, 6);

        test::check_aabb(boolean, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("subtract")
    {
    }
}

TEST_CASE("knife box")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto knife = std::make_shared<evt::node::Knife>();
    eval.AddNode(knife);

    eval.Connect({ box, 0 }, { knife, 0 });

    SECTION("keep all")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepAll);

        eval.Update();

        test::check_points_num(knife, 12);
        //test::check_edges_num(knife, 40);
        test::check_faces_num(knife, 10);

        test::check_aabb(knife, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep above")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepAbove);

        eval.Update();

        test::check_points_num(knife, 8);
        //test::check_edges_num(knife, 20);
        test::check_faces_num(knife, 5);

        test::check_aabb(knife, sm::vec3(-0.5f, 0, -0.5f), sm::vec3(0.5f, 0.5f, 0.5f));
    }

    SECTION("keep below")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepBelow);

        eval.Update();

        test::check_points_num(knife, 8);
        //test::check_edges_num(knife, 20);
        test::check_faces_num(knife, 5);

        test::check_aabb(knife, sm::vec3(-0.5f, -0.5f, -0.5f), sm::vec3(0.5f, 0, 0.5f));
    }
}

TEST_CASE("knife plane")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<evt::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(evt::GroupType::Primitives);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<evt::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(evt::GroupType::GuessFromGroup);
    blast->SetDeleteNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto knife = std::make_shared<evt::node::Knife>();
    eval.AddNode(knife);

    eval.Connect({ blast, 0 }, { knife, 0 });

    SECTION("dir x")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepAll);
        knife->SetDirection({ 1, 0, 0 });

        eval.Update();

        test::check_faces_num(knife, 2);
        test::check_points_num(knife, 6);
        test::check_edges_num(knife, 8);
    }

    SECTION("dir neg x")
    {
        knife->SetKeepType(evt::node::Knife::KeepType::KeepAll);
        knife->SetDirection({ -1, 0, 0 });

        eval.Update();

        test::check_faces_num(knife, 2);
        test::check_points_num(knife, 6);
        test::check_edges_num(knife, 8);
    }
}

TEST_CASE("normal box")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto normal = std::make_shared<evt::node::Normal>();
    eval.AddNode(normal);

    eval.Connect({ box, 0 }, { normal, 0 });

    SECTION("add to point")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Point);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 8);
        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, 8);
        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, 8);
    }

    SECTION("add to vertex")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Vertex);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_x, 24);
        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_y, 24);
        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_z, 24);
    }

    SECTION("add to primitive")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Primitive);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_x, 6);
        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_y, 6);
        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_z, 6);
    }

    SECTION("add to detail")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Detail);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_x, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_y, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_z, 1);
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_y, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
    }
}

TEST_CASE("normal plane")
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
    blast->SetGroupType(evt::GroupType::Primitives);
    blast->SetDeleteNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    auto normal = std::make_shared<evt::node::Normal>();
    eval.AddNode(normal);

    eval.Connect({ blast, 0 }, { normal, 0 });

    SECTION("add to point")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Point);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, 4);
        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, 4);
        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, 4);
        for (int i = 0; i < 4; ++i) {
            test::check_attr_value(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, i, evt::Variable(0.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, i, evt::Variable(1.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, i, evt::Variable(0.0f));
        }
    }

    SECTION("add to vertex")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Vertex);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_x, 4);
        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_y, 4);
        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_z, 4);
        for (int i = 0; i < 4; ++i) {
            test::check_attr_value(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_x, i, evt::Variable(0.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_y, i, evt::Variable(1.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_z, i, evt::Variable(0.0f));
        }
    }

    SECTION("add to primitive")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Primitive);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_x, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_y, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_z, 1);
        test::check_attr_value(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_y, 0, evt::Variable(1.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
    }

    SECTION("add to detail")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Detail);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_x, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_y, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_z, 1);
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_y, 0, evt::Variable(1.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
    }
}

TEST_CASE("normal line")
{
    test::init();

    evt::Evaluator eval;

    auto add = std::make_shared<evt::node::Add>();
    const int n = 3;
    add->SetPoints({
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
    });
    eval.AddNode(add);

    auto normal = std::make_shared<evt::node::Normal>();
    eval.AddNode(normal);

    eval.Connect({ add, 0 }, { normal, 0 });

    SECTION("add to point")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Point);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, n);
        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, n);
        test::check_attr_count(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, n);
        for (int i = 0; i < n; ++i) {
            test::check_attr_value(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_x, i, evt::Variable(0.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_y, i, evt::Variable(0.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Point, evt::GeoAttrName::norm_z, i, evt::Variable(0.0f));
        }
    }

    SECTION("add to vertex")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Vertex);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_x, n);
        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_y, n);
        test::check_attr_count(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_z, n);
        for (int i = 0; i < n; ++i) {
            test::check_attr_value(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_x, i, evt::Variable(0.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_y, i, evt::Variable(0.0f));
            test::check_attr_value(normal, evt::GeoAttrType::Vertex, evt::GeoAttrName::norm_z, i, evt::Variable(0.0f));
        }
    }

    SECTION("add to primitive")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Primitive);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_x, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_y, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_z, 1);
        test::check_attr_value(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_y, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
    }

    SECTION("add to detail")
    {
        normal->SetAttrAddTo(evt::GeoAttrType::Detail);

        eval.Update();

        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_x, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_y, 1);
        test::check_attr_count(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_z, 1);
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_y, 0, evt::Variable(0.0f));
        test::check_attr_value(normal, evt::GeoAttrType::Detail, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
    }
}

TEST_CASE("poly extrude")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    const sm::vec3 size(1, 2, 3);
    box->SetSize(size);
    eval.AddNode(box);

    SECTION("extrude the whole box")
    {
        auto extrude = std::make_shared<evt::node::PolyExtrude>();
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
        auto group = std::make_shared<evt::node::GroupCreate>();
        const std::string name("test");
        group->SetGroupName(name);
        group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
        eval.AddNode(group);

        eval.Connect({ box, 0 }, { group, 0 });

        auto extrude = std::make_shared<evt::node::PolyExtrude>();
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


TEST_CASE("poly fill")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<evt::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(evt::GroupType::Primitives);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<evt::node::Blast>();
    blast->SetGroupName("top");
    blast->SetGroupType(evt::GroupType::GuessFromGroup);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    SECTION("one face")
    {
        auto poly_fill = std::make_shared<evt::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ blast, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_faces_num(blast, 5);
        test::check_faces_num(poly_fill, 6);
    }

    SECTION("two face")
    {
        auto group2 = std::make_shared<evt::node::GroupCreate>();
        group2->SetGroupName("btm");
        group2->SetGroupType(evt::GroupType::Primitives);
        group2->EnableKeepByNormals(sm::vec3(0, -1, 0), 10);
        eval.AddNode(group2);

        eval.Connect({ blast, 0 }, { group2, 0 });

        auto blast2 = std::make_shared<evt::node::Blast>();
        blast2->SetGroupName("btm");
        blast2->SetGroupType(evt::GroupType::GuessFromGroup);
        eval.AddNode(blast2);

        eval.Connect({ group2, 0 }, { blast2, 0 });

        auto poly_fill = std::make_shared<evt::node::PolyFill>();
        eval.AddNode(poly_fill);

        eval.Connect({ blast2, 0 }, { poly_fill, 0 });

        eval.Update();

        test::check_faces_num(blast, 5);
        test::check_faces_num(blast2, 4);
        test::check_faces_num(poly_fill, 6);
    }
}
