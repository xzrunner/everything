#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/GeometryImpl.h>
#include <sop/GeoAttrDefine.h>

#include <sop/node/Box.h>
#include <sop/node/Geometry.h>
#include <sop/node/Null.h>
#include <sop/node/Measure.h>
#include <sop/node/Add.h>
#include <sop/node/Merge.h>
#include <sop/node/Normal.h>
#include <sop/node/Blast.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/AttributeWrangle.h>

#include <catch/catch.hpp>

TEST_CASE("node prop")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    eval.Update();

    auto& props = const_cast<sop::NodePropsMgr&>(box->GetProps());

    REQUIRE(props.Size() == sop::node::Box::MAX_BUILD_IN_PROP);

    props.Add("prop0", sop::Variable(1.1f));
    props.Add("prop1", sop::Variable(2));
    props.Add("prop2", sop::Variable(sm::vec3(4, 5, 6)));
    props.Add("prop3", sop::Variable(true));
    props.Add("prop4", sop::Variable("zz"));

    REQUIRE(props.Size() == sop::node::Box::MAX_BUILD_IN_PROP + 5);

    test::check_prop(box, "prop0", sop::Variable(1.1f));
    test::check_prop(box, "prop1", sop::Variable(2));
    test::check_prop(box, "prop2", sop::Variable(sm::vec3(4, 5, 6)));
    test::check_prop(box, "prop3", sop::Variable(true));
    test::check_prop(box, "prop4", sop::Variable("zz"));

    props.Remove("prop2");
    test::check_prop(box, "prop2", sop::Variable());

    REQUIRE(props.Size() == sop::node::Box::MAX_BUILD_IN_PROP + 4);

    props.Clear();
    test::check_prop(box, "prop3", sop::Variable());

    REQUIRE(props.Size() == sop::node::Box::MAX_BUILD_IN_PROP);
}

TEST_CASE("parent node prop")
{
    test::init();

    sop::Evaluator eval;

    auto root = std::make_shared<sop::node::Geometry>();
    root->SetName("root");
    eval.AddNode(root);

    auto geo = std::make_shared<sop::node::Geometry>();
    geo->SetName("geo");
    eval.AddNode(geo);

    sop::node::Geometry::AddChild(root, geo);

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    sop::node::Geometry::AddChild(geo, box);

    auto& geo_props = const_cast<sop::NodePropsMgr&>(geo->GetProps());
    geo_props.Add("BoxLength", sop::Variable(8));
    geo_props.Add("BoxWidth",  sop::Variable(5));
    geo_props.Add("BoxHeight", sop::Variable(3));

    SECTION("direct props")
    {
        auto& box_props = const_cast<sop::NodePropsMgr&>(box->GetProps());
        box_props.SetExpr(sop::node::Box::SIZE_X, "ch(\"../BoxLength\")");
        box_props.SetExpr(sop::node::Box::SIZE_Y, "ch(\"../BoxHeight\")");
        box_props.SetExpr(sop::node::Box::SIZE_Z, "ch(\"../BoxWidth\")");
        box_props.SetExpr(sop::node::Box::POS_Y,  "ch(\"sizey\")/2");

        eval.Update();

        test::check_aabb(box, sm::vec3(-4, 0, -2.5f), sm::vec3(4, 3, 2.5f));
    }

    SECTION("node to props")
    {
        auto& box_props = const_cast<sop::NodePropsMgr&>(box->GetProps());
        box_props.SetExpr(sop::node::Box::SIZE_X, "ch(\"../../geo/BoxLength\")");
        box_props.SetExpr(sop::node::Box::SIZE_Y, "ch(\"../../geo/BoxHeight\")");
        box_props.SetExpr(sop::node::Box::SIZE_Z, "ch(\"../../geo/BoxWidth\")");
        box_props.SetExpr(sop::node::Box::POS_Y,  "ch(\"sizey\")/2");

        eval.Update();

        test::check_aabb(box, sm::vec3(-4, 0, -2.5f), sm::vec3(4, 3, 2.5f));
    }
}

TEST_CASE("use other's prop")
{
    test::init();

    sop::Evaluator eval;

    auto null = std::make_shared<sop::node::Null>();
    null->SetName("controller");
    auto& null_props = const_cast<sop::NodePropsMgr&>(null->GetProps());
    const int h_idx = null_props.Add("Height", sop::Variable(3.0f));
    const int w_idx = null_props.Add("Width",  sop::Variable(8.0f));
    const int l_idx = null_props.Add("Length", sop::Variable(5.0f));
    eval.AddNode(null);

    auto box = std::make_shared<sop::node::Box>();
    auto& box_props = const_cast<sop::NodePropsMgr&>(box->GetProps());
    box_props.SetExpr(sop::node::Box::SIZE_X, "ch(\"../controller/Width\")");
    box_props.SetExpr(sop::node::Box::SIZE_Y, "ch(\"../controller/Height\")");
    box_props.SetExpr(sop::node::Box::SIZE_Z, "ch(\"../controller/Length\")");
    eval.AddNode(box);

    eval.Connect({ null, 0 }, { box, 0 });

    {
        eval.Update();

        test::check_aabb(box, sm::vec3(-4, -1.5f, -2.5f), sm::vec3(4, 1.5f, 2.5f));
    }
    {
        null_props.SetValue(h_idx, sop::Variable(4.0f));

        eval.MakeDirty();
        eval.Update();

        test::check_aabb(box, sm::vec3(-4, -2, -2.5f), sm::vec3(4, 2, 2.5f));
    }
}

TEST_CASE("attr combine")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 0, 2, 1 },
    });
    eval.AddNode(add);

    auto measure1 = std::make_shared<sop::node::Measure>();
    eval.Connect({ box, 0 }, { measure1, 0 });
    eval.AddNode(measure1);

    auto measure2 = std::make_shared<sop::node::Measure>();
    eval.Connect({ add, 0 }, { measure2, 0 });
    eval.AddNode(measure2);

    auto merge = std::make_shared<sop::node::Merge>();
    eval.AddNode(merge);

    eval.Connect({ measure1, 0 }, { merge, sop::node::Merge::IDX_SRC_A });
    eval.Connect({ measure2, 0 }, { merge, sop::node::Merge::IDX_SRC_B });

    SECTION("perimeter")
    {
        measure1->SetMesureType(sop::node::Measure::Type::Perimeter);
        measure2->SetMesureType(sop::node::Measure::Type::Perimeter);

        eval.Update();

        test::check_attr_count(merge, sop::GeoAttrClass::Primitive, "perimeter", 7);
        test::check_attr_value(merge, sop::GeoAttrClass::Primitive, "perimeter", 3, sop::Variable(4.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Primitive, "perimeter", 4, sop::Variable(4.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Primitive, "perimeter", 6, sop::Variable(3.0f));
    }

    SECTION("area")
    {
        measure1->SetMesureType(sop::node::Measure::Type::Area);
        measure2->SetMesureType(sop::node::Measure::Type::Area);

        eval.Update();

        test::check_attr_count(merge, sop::GeoAttrClass::Primitive, "area", 7);
        test::check_attr_value(merge, sop::GeoAttrClass::Primitive, "area", 2, sop::Variable(1.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Primitive, "area", 3, sop::Variable(1.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Primitive, "area", 6, sop::Variable(0.0f));
    }
}

TEST_CASE("attr combine 2")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    eval.AddNode(box0);

    auto attr0 = std::make_shared<sop::node::AttributeWrangle>();
    eval.AddNode(attr0);

    eval.Connect({ box0, 0 }, { attr0, 0 });

    auto box1 = std::make_shared<sop::node::Box>();
    eval.AddNode(box1);

    auto attr1 = std::make_shared<sop::node::AttributeWrangle>();
    eval.AddNode(attr1);

    eval.Connect({ box1, 0 }, { attr1, 0 });

    auto merge = std::make_shared<sop::node::Merge>();
    eval.AddNode(merge);

    eval.Connect({ attr0, 0 }, { merge, sop::node::Merge::IDX_SRC_A });
    eval.Connect({ attr1, 0 }, { merge, sop::node::Merge::IDX_SRC_B });

    SECTION("simple")
    {
        attr0->SetVexExpr(R"(
setattrib(0, "point", "test0", @ptnum, 0, @ptnum);
)");
        attr1->SetVexExpr(R"(
setattrib(0, "point", "test1", @ptnum, 0, -@ptnum);
)");

        eval.Update();

        test::check_attr_count(merge, sop::GeoAttrClass::Point, "test0", 16);
        test::check_attr_count(merge, sop::GeoAttrClass::Point, "test1", 16);
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test0", 3,  sop::Variable(3.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test0", 11, sop::Variable(0.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test1", 5,  sop::Variable(0.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test1", 13, sop::Variable(-5.0f));
    }

    SECTION("same name")
    {
        attr0->SetVexExpr(R"(
setattrib(0, "point", "test0", @ptnum, 0, @ptnum);
)");
        attr1->SetVexExpr(R"(
setattrib(0, "point", "test0", @ptnum, 0, -@ptnum);
)");

        eval.Update();

        test::check_attr_count(merge, sop::GeoAttrClass::Point, "test0", 16);
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test0", 3,  sop::Variable(3.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test0", 11, sop::Variable(-3.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test0", 5,  sop::Variable(5.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test0", 13, sop::Variable(-5.0f));
    }

    SECTION("part same")
    {
        attr0->SetVexExpr(R"(
setattrib(0, "point", "test0", @ptnum, 0, @ptnum);
)");
        attr1->SetVexExpr(R"(
if (@ptnum == 4) {
    setattrib(0, "point", "test0", @ptnum, 0, 1.1);
} else {
    setattrib(0, "point", "test0", @ptnum, 0, @ptnum);
}
setattrib(0, "point", "test1", @ptnum, 0, -@ptnum);
)");

        eval.Update();

        test::check_attr_count(merge, sop::GeoAttrClass::Point, "test0", 16);
        test::check_attr_count(merge, sop::GeoAttrClass::Point, "test1", 16);
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test0", 12, sop::Variable(1.1f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test1",  4, sop::Variable(0.0f));
        test::check_attr_value(merge, sop::GeoAttrClass::Point, "test1", 12, sop::Variable(-4.0f));
    }
}

TEST_CASE("attr blast")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto normal = std::make_shared<sop::node::Normal>();
    normal->SetAttrAddTo(sop::GeoAttrClass::Primitive);
    eval.AddNode(normal);

    eval.Connect({ box, 0 }, { normal, 0 });

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("Top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->EnableKeepByNormals({ 0, 1, 0 }, 10);
    eval.AddNode(group);

    eval.Connect({ normal, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("Top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDeleteNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    eval.Update();

    test::check_attr_count(blast, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1);
    test::check_attr_value(blast, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 1, 0)));
}