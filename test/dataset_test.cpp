#include "utility.h"

#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>
#include <everything/GeoAttrName.h>

#include <everything/node/Box.h>
#include <everything/node/Geometry.h>
#include <everything/node/Null.h>
#include <everything/node/Measure.h>
#include <everything/node/Add.h>
#include <everything/node/Merge.h>
#include <everything/node/Normal.h>
#include <everything/node/Blast.h>
#include <everything/node/GroupCreate.h>

#include <catch/catch.hpp>

TEST_CASE("node prop")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    eval.Update();

    auto& props = const_cast<evt::NodePropsMgr&>(box->GetProps());

    REQUIRE(props.Size() == evt::node::Box::MAX_BUILD_IN_PROP);

    props.Add("prop0", evt::Variable(1.1f));
    props.Add("prop1", evt::Variable(2));
    props.Add("prop2", evt::Variable(sm::vec3(4, 5, 6)));
    props.Add("prop3", evt::Variable(true));
    props.Add("prop4", evt::Variable("zz"));

    REQUIRE(props.Size() == evt::node::Box::MAX_BUILD_IN_PROP + 5);

    test::check_prop(box, "prop0", evt::Variable(1.1f));
    test::check_prop(box, "prop1", evt::Variable(2));
    test::check_prop(box, "prop2", evt::Variable(sm::vec3(4, 5, 6)));
    test::check_prop(box, "prop3", evt::Variable(true));
    test::check_prop(box, "prop4", evt::Variable("zz"));

    props.Remove("prop2");
    test::check_prop(box, "prop2", evt::Variable());

    REQUIRE(props.Size() == evt::node::Box::MAX_BUILD_IN_PROP + 4);

    props.Clear();
    test::check_prop(box, "prop3", evt::Variable());

    REQUIRE(props.Size() == evt::node::Box::MAX_BUILD_IN_PROP);
}

TEST_CASE("parent node prop")
{
    test::init();

    evt::Evaluator eval;

    auto root = std::make_shared<evt::node::Geometry>();
    root->SetName("root");
    eval.AddNode(root);

    auto geo = std::make_shared<evt::node::Geometry>();
    geo->SetName("geo");
    eval.AddNode(geo);

    evt::node::Geometry::AddChild(root, geo);

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    evt::node::Geometry::AddChild(geo, box);

    auto& geo_props = const_cast<evt::NodePropsMgr&>(geo->GetProps());
    geo_props.Add("BoxLength", evt::Variable(8));
    geo_props.Add("BoxWidth",  evt::Variable(5));
    geo_props.Add("BoxHeight", evt::Variable(3));

    SECTION("direct props")
    {
        auto& box_props = const_cast<evt::NodePropsMgr&>(box->GetProps());
        box_props.SetExpr(evt::node::Box::SIZE_X, "ch(\"../BoxLength\")");
        box_props.SetExpr(evt::node::Box::SIZE_Y, "ch(\"../BoxHeight\")");
        box_props.SetExpr(evt::node::Box::SIZE_Z, "ch(\"../BoxWidth\")");
        box_props.SetExpr(evt::node::Box::POS_Y,  "ch(\"sizey\")/2");

        eval.Update();

        test::check_aabb(box, sm::vec3(-4, 0, -2.5f), sm::vec3(4, 3, 2.5f));
    }

    SECTION("node to props")
    {
        auto& box_props = const_cast<evt::NodePropsMgr&>(box->GetProps());
        box_props.SetExpr(evt::node::Box::SIZE_X, "ch(\"../../geo/BoxLength\")");
        box_props.SetExpr(evt::node::Box::SIZE_Y, "ch(\"../../geo/BoxHeight\")");
        box_props.SetExpr(evt::node::Box::SIZE_Z, "ch(\"../../geo/BoxWidth\")");
        box_props.SetExpr(evt::node::Box::POS_Y,  "ch(\"sizey\")/2");

        eval.Update();

        test::check_aabb(box, sm::vec3(-4, 0, -2.5f), sm::vec3(4, 3, 2.5f));
    }
}

TEST_CASE("use other's prop")
{
    test::init();

    evt::Evaluator eval;

    auto null = std::make_shared<evt::node::Null>();
    null->SetName("controller");
    auto& null_props = const_cast<evt::NodePropsMgr&>(null->GetProps());
    const int h_idx = null_props.Add("Height", evt::Variable(3.0f));
    const int w_idx = null_props.Add("Width",  evt::Variable(8.0f));
    const int l_idx = null_props.Add("Length", evt::Variable(5.0f));
    eval.AddNode(null);

    auto box = std::make_shared<evt::node::Box>();
    auto& box_props = const_cast<evt::NodePropsMgr&>(box->GetProps());
    box_props.SetExpr(evt::node::Box::SIZE_X, "ch(\"../controller/Width\")");
    box_props.SetExpr(evt::node::Box::SIZE_Y, "ch(\"../controller/Height\")");
    box_props.SetExpr(evt::node::Box::SIZE_Z, "ch(\"../controller/Length\")");
    eval.AddNode(box);

    eval.Connect({ null, 0 }, { box, 0 });

    {
        eval.Update();

        test::check_aabb(box, sm::vec3(-4, -1.5f, -2.5f), sm::vec3(4, 1.5f, 2.5f));
    }
    {
        null_props.SetValue(h_idx, evt::Variable(4.0f));

        eval.MakeDirty();
        eval.Update();

        test::check_aabb(box, sm::vec3(-4, -2, -2.5f), sm::vec3(4, 2, 2.5f));
    }
}

TEST_CASE("attr combine")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto add = std::make_shared<evt::node::Add>();
    add->SetPoints({
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 0, 2, 1 },
    });
    eval.AddNode(add);

    auto measure1 = std::make_shared<evt::node::Measure>();
    eval.Connect({ box, 0 }, { measure1, 0 });
    eval.AddNode(measure1);

    auto measure2 = std::make_shared<evt::node::Measure>();
    eval.Connect({ add, 0 }, { measure2, 0 });
    eval.AddNode(measure2);

    auto merge = std::make_shared<evt::node::Merge>();
    eval.AddNode(merge);

    eval.Connect({ measure1, 0 }, { merge, evt::node::Merge::IDX_SRC_A });
    eval.Connect({ measure2, 0 }, { merge, evt::node::Merge::IDX_SRC_B });

    SECTION("perimeter")
    {
        measure1->SetMesureType(evt::node::Measure::Type::Perimeter);
        measure2->SetMesureType(evt::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(merge, evt::GeoAttrType::Primitive, "perimeter", 7);
        test::check_attr_value(merge, evt::GeoAttrType::Primitive, "perimeter", 3, evt::Variable(4.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Primitive, "perimeter", 4, evt::Variable(4.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Primitive, "perimeter", 6, evt::Variable(3.0f));
    }

    SECTION("area")
    {
        measure1->SetMesureType(evt::node::Measure::Type::Area);
        measure2->SetMesureType(evt::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(merge, evt::GeoAttrType::Primitive, "area", 7);
        test::check_attr_value(merge, evt::GeoAttrType::Primitive, "area", 2, evt::Variable(1.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Primitive, "area", 3, evt::Variable(1.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Primitive, "area", 6, evt::Variable(0.0f));
    }
}

TEST_CASE("attr combine 2")
{
    test::init();

    evt::Evaluator eval;

    auto box0 = std::make_shared<evt::node::Box>();
    eval.AddNode(box0);

    auto box1 = std::make_shared<evt::node::Box>();
    eval.AddNode(box1);

    auto merge = std::make_shared<evt::node::Merge>();
    eval.AddNode(merge);

    eval.Connect({ box0, 0 }, { merge, evt::node::Merge::IDX_SRC_A });
    eval.Connect({ box1, 0 }, { merge, evt::node::Merge::IDX_SRC_B });

    // create geo impl
    eval.Update();
    eval.MakeDirty(false);
    merge->SetDirty(true);

    std::vector<evt::VarValue> var_list0;
    var_list0.reserve(8);
    for (int i = 0; i < 8; ++i) {
        var_list0.push_back(evt::VarValue(static_cast<float>(i)));
    }

    std::vector<evt::VarValue> var_list1;
    var_list1.reserve(8);
    for (int i = 0; i < 8; ++i) {
        var_list1.push_back(evt::VarValue(static_cast<float>(-i)));
    }

    SECTION("simple")
    {
        box0->GetGeometry()->GetAttr().AddAttr(
            evt::GeoAttrType::Point, { "test0", evt::VarType::Float }, var_list0
        );

        box1->GetGeometry()->GetAttr().AddAttr(
            evt::GeoAttrType::Point, { "test1", evt::VarType::Float }, var_list1
        );

        eval.Update();

        test::check_attr_count(merge, evt::GeoAttrType::Point, "test0", 16);
        test::check_attr_count(merge, evt::GeoAttrType::Point, "test1", 16);
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test0", 3,  evt::Variable(3.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test0", 11, evt::Variable(0.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test1", 5,  evt::Variable(0.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test1", 13, evt::Variable(-5.0f));
    }

    SECTION("same name")
    {
        box0->GetGeometry()->GetAttr().AddAttr(
            evt::GeoAttrType::Point, { "test0", evt::VarType::Float }, var_list0
        );

        box1->GetGeometry()->GetAttr().AddAttr(
            evt::GeoAttrType::Point, { "test0", evt::VarType::Float }, var_list1
        );

        eval.Update();

        test::check_attr_count(merge, evt::GeoAttrType::Point, "test0", 16);
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test0", 3,  evt::Variable(3.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test0", 11, evt::Variable(-3.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test0", 5,  evt::Variable(5.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test0", 13, evt::Variable(-5.0f));
    }

    SECTION("part same")
    {
        auto var_list2 = var_list0;
        var_list2[4].f = 1.1f;

        box0->GetGeometry()->GetAttr().AddAttr(
            evt::GeoAttrType::Point, { "test0", evt::VarType::Float }, var_list0
        );

        box1->GetGeometry()->GetAttr().AddAttr(
            evt::GeoAttrType::Point, { "test0", evt::VarType::Float }, var_list2
        );
        box1->GetGeometry()->GetAttr().AddAttr(
            evt::GeoAttrType::Point, { "test1", evt::VarType::Float }, var_list1
        );

        eval.Update();

        test::check_attr_count(merge, evt::GeoAttrType::Point, "test0", 16);
        test::check_attr_count(merge, evt::GeoAttrType::Point, "test1", 16);
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test0", 12, evt::Variable(1.1f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test1",  4, evt::Variable(0.0f));
        test::check_attr_value(merge, evt::GeoAttrType::Point, "test1", 12, evt::Variable(-4.0f));
    }
}

TEST_CASE("attr blast")
{
    test::init();

    evt::Evaluator eval;

    auto box = std::make_shared<evt::node::Box>();
    eval.AddNode(box);

    auto normal = std::make_shared<evt::node::Normal>();
    normal->SetAttrAddTo(evt::GeoAttrType::Primitive);
    eval.AddNode(normal);

    eval.Connect({ box, 0 }, { normal, 0 });

    auto group = std::make_shared<evt::node::GroupCreate>();
    group->SetGroupName("Top");
    group->SetGroupType(evt::GroupType::Primitives);
    group->EnableKeepByNormals({ 0, 1, 0 }, 10);
    eval.AddNode(group);

    eval.Connect({ normal, 0 }, { group, 0 });

    auto blast = std::make_shared<evt::node::Blast>();
    blast->SetGroupName("Top");
    blast->SetGroupType(evt::GroupType::GuessFromGroup);
    blast->SetDeleteNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    eval.Update();

    test::check_attr_count(blast, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_x, 1);
    test::check_attr_count(blast, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_y, 1);
    test::check_attr_count(blast, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_z, 1);
    test::check_attr_value(blast, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_x, 0, evt::Variable(0.0f));
    test::check_attr_value(blast, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_y, 0, evt::Variable(1.0f));
    test::check_attr_value(blast, evt::GeoAttrType::Primitive, evt::GeoAttrName::norm_z, 0, evt::Variable(0.0f));
}