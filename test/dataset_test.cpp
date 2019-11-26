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

TEST_CASE("node parm")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    eval.Update();

    auto& parms = const_cast<sop::NodeParmsMgr&>(box->GetParms());

    parms.AddParm("parm0", sop::Variable(1.1f));
    parms.AddParm("parm1", sop::Variable(2));
    parms.AddParm("parm2", sop::Variable(sm::vec3(4, 5, 6)));
    parms.AddParm("parm3", sop::Variable(true));
    parms.AddParm("parm4", sop::Variable("zz"));

    REQUIRE(parms.ExternalParmSize() == 5);

    test::check_parm(box, "parm0", sop::Variable(1.1f));
    test::check_parm(box, "parm1", sop::Variable(2));
    test::check_parm(box, "parm2", sop::Variable(sm::vec3(4, 5, 6)));
    test::check_parm(box, "parm3", sop::Variable(true));
    test::check_parm(box, "parm4", sop::Variable("zz"));

    parms.RemoveParm("parm2");
    test::check_parm(box, "parm2", sop::Variable());

    REQUIRE(parms.ExternalParmSize() == 4);

    parms.ClearAllParms();
    test::check_parm(box, "parm3", sop::Variable());

    REQUIRE(parms.ExternalParmSize() == 0);
}

TEST_CASE("parent node parm")
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

    auto& geo_parms = const_cast<sop::NodeParmsMgr&>(geo->GetParms());
    geo_parms.AddParm("BoxLength", sop::Variable(8));
    geo_parms.AddParm("BoxWidth",  sop::Variable(5));
    geo_parms.AddParm("BoxHeight", sop::Variable(3));

    SECTION("direct parms")
    {
        auto& box_parms = const_cast<sop::NodeParmsMgr&>(box->GetParms());
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
        //    "ch(\"../BoxLength\")", 0);
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
        //    "ch(\"../BoxHeight\")", 1);
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
        //    "ch(\"../BoxWidth\")",  2);
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Center),
        //    "ch(\"sizey\")/2",      1);
        box_parms.SetExpr("sizex", "ch(\"../BoxLength\")");
        box_parms.SetExpr("sizey", "ch(\"../BoxHeight\")");
        box_parms.SetExpr("sizez", "ch(\"../BoxWidth\")");
        box_parms.SetExpr("ty", "ch(\"sizey\")/2");

        eval.Update();

        test::check_aabb(box, sm::vec3(-4, 0, -2.5f), sm::vec3(4, 3, 2.5f));
    }

    SECTION("node to parms")
    {
        auto& box_parms = const_cast<sop::NodeParmsMgr&>(box->GetParms());
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
        //    "ch(\"../../geo/BoxLength\")", 0);
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
        //    "ch(\"../../geo/BoxHeight\")", 1);
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
        //    "ch(\"../../geo/BoxWidth\")",  2);
        //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Center),
        //    "ch(\"sizey\")/2",             1);
        box_parms.SetExpr("sizex", "ch(\"../../geo/BoxLength\")");
        box_parms.SetExpr("sizey", "ch(\"../../geo/BoxHeight\")");
        box_parms.SetExpr("sizez", "ch(\"../../geo/BoxWidth\")");
        box_parms.SetExpr("ty", "ch(\"sizey\")/2");

        eval.Update();

        test::check_aabb(box, sm::vec3(-4, 0, -2.5f), sm::vec3(4, 3, 2.5f));
    }
}

TEST_CASE("use other's parm")
{
    test::init();

    sop::Evaluator eval;

    auto null = std::make_shared<sop::node::Null>();
    null->SetName("controller");
    auto& null_parms = const_cast<sop::NodeParmsMgr&>(null->GetParms());
    null_parms.AddParm("Height", sop::Variable(3.0f));
    null_parms.AddParm("Width",  sop::Variable(8.0f));
    null_parms.AddParm("Length", sop::Variable(5.0f));
    eval.AddNode(null);

    auto box = std::make_shared<sop::node::Box>();
    auto& box_parms = const_cast<sop::NodeParmsMgr&>(box->GetParms());
    //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
    //    "ch(\"../controller/Width\")",  0);
    //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
    //    "ch(\"../controller/Height\")", 1);
    //box_parms.SetExpr(static_cast<int>(sop::node::Box::Parm::Size),
    //    "ch(\"../controller/Length\")", 2);
    box_parms.SetExpr("sizex", "ch(\"../controller/Width\")");
    box_parms.SetExpr("sizey", "ch(\"../controller/Height\")");
    box_parms.SetExpr("sizez", "ch(\"../controller/Length\")");
    eval.AddNode(box);

    eval.Connect({ null, 0 }, { box, 0 });

    {
        eval.Update();

        test::check_aabb(box, sm::vec3(-4, -1.5f, -2.5f), sm::vec3(4, 1.5f, 2.5f));
    }
    {
        null_parms.SetValue("Height", sop::Variable(4.0f));

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
    add->SetPointsEnable(std::vector<bool>(add->GetPoints().size(), true));
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
    normal->SetAttrAddTo(sop::node::Normal::AddToType::Primitives);
    eval.AddNode(normal);

    eval.Connect({ box, 0 }, { normal, 0 });

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("Top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->SetKeepByNormals(true);
    group->SetKeepByNormalsDir(sm::vec3(0, 1, 0));
    group->SetKeepByNormalsAngle(10.0f);
    eval.AddNode(group);

    eval.Connect({ normal, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName("Top");
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    blast->SetDelNonSelected(true);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    eval.Update();

    test::check_attr_count(blast, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 1);
    test::check_attr_value(blast, sop::GeoAttrClass::Primitive, sop::GeoAttrNames[sop::GEO_ATTR_NORM], 0, sop::Variable(sm::vec3(0, 1, 0)));
}