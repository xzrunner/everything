#include "utility.h"

#include <sop/Evaluator.h>
#include <sop/GeometryImpl.h>
#include <sop/GeoAttrDefine.h>

#include <sop/node/Blast.h>
#include <sop/node/CopyToPoints.h>
#include <sop/node/ForeachPrimBegin.h>
#include <sop/node/ForeachPrimEnd.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/Merge.h>
#include <sop/node/Null.h>
#include <sop/node/Split.h>
#include <sop/node/Switch.h>

#include <sop/node/Box.h>
#include <sop/node/PolyExtrude.h>
#include <sop/node/Delete.h>
#include <sop/node/Add.h>
#include <sop/node/Carve.h>
#include <sop/node/Color.h>
#include <sop/node/Normal.h>
#include <sop/node/AttributeCreate.h>

#include <catch/catch.hpp>

TEST_CASE("blast")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    const sm::vec3 size(1, 2, 3);
    box->SetSize(size);
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    const std::string name("test");
    group->SetGroupName(name);
    group->SetGroupType(sop::GroupType::Primitives);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto blast = std::make_shared<sop::node::Blast>();
    blast->SetGroupName(name);
    blast->SetGroupType(sop::GroupType::GuessFromGroup);
    eval.AddNode(blast);

    eval.Connect({ group, 0 }, { blast, 0 });

    SECTION("del selected")
    {
        eval.Update();

        test::check_faces_num(blast, 5);
        test::check_group_num(blast, name, 0);
    }

    SECTION("del non selected")
    {
        blast->SetDeleteNonSelected(true);

        eval.Update();

        test::check_faces_num(blast, 1);
        test::check_group_num(blast, name, 1);
    }
}

TEST_CASE("copy to points")
{
    test::init();

    sop::Evaluator eval;

    auto src_box = std::make_shared<sop::node::Box>();
    const sm::vec3 src_size(1, 2, 3);
    src_box->SetSize(src_size);
    const sm::vec3 src_pos(21, 22, 23);
    src_box->SetCenter(src_pos);
    eval.AddNode(src_box);

    auto target_box = std::make_shared<sop::node::Box>();
    const sm::vec3 target_pos(5, 6, 7);
    const sm::vec3 target_size(1.1f, 2.2f, 3.3f);
    target_box->SetCenter(target_pos);
    target_box->SetSize(target_size);
    eval.AddNode(target_box);

    auto copy = std::make_shared<sop::node::CopyToPoints>();
    eval.AddNode(copy);

    eval.Connect({ src_box, 0 },    { copy, sop::node::CopyToPoints::IDX_SRC_PRIM });
    eval.Connect({ target_box, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

    eval.Update();

    auto h_src_sz = src_size * 0.5f;
    auto h_tar_sz = target_size * 0.5f;
    test::check_aabb(copy, src_pos + target_pos - h_src_sz - h_tar_sz, src_pos + target_pos + h_src_sz + h_tar_sz);
}

TEST_CASE("copy to points with points dir")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    box->SetSize({ 1, 2, 4 });
    eval.AddNode(box);

    auto copy = std::make_shared<sop::node::CopyToPoints>();
    copy->EnableUsePointDir(true);
    eval.AddNode(copy);

    eval.Connect({ box, 0 }, { copy, sop::node::CopyToPoints::IDX_SRC_PRIM });

    SECTION("shape")
    {
        auto line = std::make_shared<sop::node::Add>();
        line->SetPoints({
            { 0, 0, 0 },
            { 2, 4, 8 }
        });
        eval.AddNode(line);

        eval.Connect({ line, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -0.5f, -1, -2 }, { 2.5f, 5, 10 });
    }

    SECTION("box")
    {
        auto to_box = std::make_shared<sop::node::Box>();
        to_box->SetSize({ 6, 6, 6 });
        eval.AddNode(to_box);

        eval.Connect({ to_box, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -5.04904f, -5.04904f, -5.02073f }, { 5.04904f, 5.04904f, 5.02073f });
    }

    SECTION("plane")
    {
        auto to_box = std::make_shared<sop::node::Box>();
        to_box->SetSize({ 6, 6, 6 });
        eval.AddNode(to_box);

        auto group = std::make_shared<sop::node::GroupCreate>();
        group->SetGroupName("Top");
        group->SetGroupType(sop::GroupType::Primitives);
        group->EnableKeepByNormals({ 0, 1, 0 }, 10);
        eval.AddNode(group);

        eval.Connect({ to_box, 0 }, { group, 0 });

        auto blast = std::make_shared<sop::node::Blast>();
        blast->SetGroupName("Top");
        blast->SetGroupType(sop::GroupType::GuessFromGroup);
        blast->SetDeleteNonSelected(true);
        eval.AddNode(blast);

        eval.Connect({ group, 0 }, { blast, 0 });

        eval.Connect({ blast, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -3.5f, 1, -4 }, { 3.5f, 5, 4 });
    }

    SECTION("plane with normal")
    {
        auto to_box = std::make_shared<sop::node::Box>();
        to_box->SetSize({ 6, 6, 6 });
        eval.AddNode(to_box);

        auto normal = std::make_shared<sop::node::Normal>();
        normal->SetAttrAddTo(sop::GeoAttrClass::Point);
        eval.AddNode(normal);

        eval.Connect({ to_box, 0 }, { normal, 0 });

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

        eval.Connect({ blast, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

        eval.Update();

        test::check_aabb(copy, { -5.04904f, 0.950962f, -5.02073f }, { 5.04904f, 5.04904f, 5.02073f });
    }
}

TEST_CASE("copy to points with attr")
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
    add->SetPoints({
        { 0, 0, 0 },
        { 10, 10, 10 },
    });
    eval.AddNode(add);

    auto copy = std::make_shared<sop::node::CopyToPoints>();
    eval.AddNode(copy);

    eval.Connect({ color, 0 }, { copy, sop::node::CopyToPoints::IDX_SRC_PRIM });
    eval.Connect({ add, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

    eval.Update();

    test::check_points_num(copy, 16);
    test::check_attr_count(copy, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 16);
    test::check_attr_value(copy, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 2,  sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    test::check_attr_value(copy, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 7,  sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
    test::check_attr_value(copy, sop::GeoAttrClass::Point, sop::GeoAttrNames[sop::GEO_ATTR_CD], 13, sop::Variable(sm::vec3(0.3f, 0.4f, 0.5f)));
}

TEST_CASE("copy to points with default attr scale")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({ { 0, 0, 0 } });
    eval.AddNode(add);

    auto attr_create = std::make_shared<sop::node::AttributeCreate>();
    std::vector<sop::node::AttributeCreate::Item> items;
    items.emplace_back(sop::GeoAttrNames[sop::GEO_ATTR_PSCALE], sop::node::AttributeCreate::ItemType::Float,
        sop::GeoAttrClass::Point, sop::VarValue(2.0f), sop::VarValue(0.0f));
    attr_create->SetAttrItems(items);
    eval.AddNode(attr_create);

    eval.Connect({ add, 0 }, { attr_create, 0 });

    auto copy = std::make_shared<sop::node::CopyToPoints>();
    eval.AddNode(copy);

    eval.Connect({ box, 0 },         { copy, sop::node::CopyToPoints::IDX_SRC_PRIM });
    eval.Connect({ attr_create, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

    eval.Update();

    test::check_aabb(copy, sm::vec3(-1, -1, -1), sm::vec3(1, 1, 1));
}

TEST_CASE("copy to points with default attr N up")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    box->SetSize({ 1, 2, 4 });
    eval.AddNode(box);

    auto add = std::make_shared<sop::node::Add>();
    add->SetPoints({ { 0, 0, 0 } });
    eval.AddNode(add);

    auto attr_create = std::make_shared<sop::node::AttributeCreate>();
    std::vector<sop::node::AttributeCreate::Item> items;
    //items.emplace_back(sop::GEO_ATTR_NORM, sop::GeoAttrClass::Point, sop::VarValue(sm::vec3(0, 1, 1)));
    //items.emplace_back(sop::GEO_ATTR_UP,   sop::GeoAttrClass::Point, sop::VarValue(sm::vec3(1, 1, 0)));
    items.emplace_back(sop::GeoAttrNames[sop::GEO_ATTR_NORM], sop::node::AttributeCreate::ItemType::Vector, 
        sop::GeoAttrClass::Point, sop::VarValue(sm::vec3(0, 0, 1)), sop::VarValue(sm::vec3(0, 0, 0)));
    items.emplace_back(sop::GeoAttrNames[sop::GEO_ATTR_UP], sop::node::AttributeCreate::ItemType::Vector, 
        sop::GeoAttrClass::Point, sop::VarValue(sm::vec3(1, 1, 0)), sop::VarValue(sm::vec3(0, 0, 0)));
    //items.emplace_back(sop::GEO_ATTR_NORM, sop::GeoAttrClass::Point, sop::VarValue(sm::vec3(0, 1, 1)));
    //items.emplace_back(sop::GEO_ATTR_UP,   sop::GeoAttrClass::Point, sop::VarValue(sm::vec3(0, 1, 0)));
    attr_create->SetAttrItems(items);
    eval.AddNode(attr_create);

    eval.Connect({ add, 0 }, { attr_create, 0 });

    auto copy = std::make_shared<sop::node::CopyToPoints>();
    eval.AddNode(copy);

    eval.Connect({ box, 0 }, { copy, sop::node::CopyToPoints::IDX_SRC_PRIM });
    eval.Connect({ attr_create, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

    eval.Update();

//    test::check_aabb(copy, sm::vec3(-1.10517f, -2.11114f, -2.11114f), sm::vec3(1.10517f, 2.11114f, 2.11114f));
    test::check_aabb(copy, sm::vec3(-1.06066f, -1.06066f, -2), sm::vec3(1.06066f, 1.06066f, 2));
//    test::check_aabb(copy, sm::vec3(-0.5f, -2.12132f, -2.12132f), sm::vec3(0.5f, 2.12132f, 2.12132f));
}

TEST_CASE("foreach primitive")
{
    test::init();

    sop::Evaluator eval;

    auto src_box = std::make_shared<sop::node::Box>();
    eval.AddNode(src_box);

    // cut top face
    auto top_face_group = std::make_shared<sop::node::GroupCreate>();
    top_face_group->SetGroupName("top_face");
    top_face_group->SetGroupType(sop::GroupType::Primitives);
    top_face_group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(top_face_group);
    hdiop::make_connecting<sop::NodeVarType>({ src_box, 0 }, { top_face_group, 0 });
    auto top_face_blast = std::make_shared<sop::node::Blast>();
    top_face_blast->SetGroupName("top_face");
    top_face_blast->SetGroupType(sop::GroupType::GuessFromGroup);
    eval.AddNode(top_face_blast);
    hdiop::make_connecting<sop::NodeVarType>({ top_face_group, 0 }, { top_face_blast, 0 });

    // cut bottom face
    auto btm_face_group = std::make_shared<sop::node::GroupCreate>();
    btm_face_group->SetGroupName("btm_face");
    btm_face_group->SetGroupType(sop::GroupType::Primitives);
    btm_face_group->EnableKeepByNormals(sm::vec3(0, -1, 0), 10);
    eval.AddNode(btm_face_group);
    hdiop::make_connecting<sop::NodeVarType>({ top_face_blast, 0 }, { btm_face_group, 0 });
    auto btm_face_blast = std::make_shared<sop::node::Blast>();
    btm_face_blast->SetGroupName("btm_face");
    btm_face_blast->SetGroupType(sop::GroupType::GuessFromGroup);
    eval.AddNode(btm_face_blast);
    hdiop::make_connecting<sop::NodeVarType>({ btm_face_group, 0 }, { btm_face_blast, 0 });

    auto foreach_begin = std::make_shared<sop::node::ForeachPrimBegin>();
    eval.AddNode(foreach_begin);

    hdiop::make_connecting<sop::NodeVarType>({ btm_face_blast, 0 }, { foreach_begin, 0 });

    auto del = std::make_shared<sop::node::Delete>();
    del->SetFilterExpr("@P.y > 0");
    eval.AddNode(del);

    hdiop::make_connecting<sop::NodeVarType>({ foreach_begin, 0 }, { del, 0 });

    auto add = std::make_shared<sop::node::Add>();
    eval.AddNode(add);

    hdiop::make_connecting<sop::NodeVarType>({ del, 0 }, { add, 0 });

    auto carve = std::make_shared<sop::node::Carve>();
    carve->SetFirstU(0.25f);
    carve->SetSecondU(0.75f);
    eval.AddNode(carve);

    hdiop::make_connecting<sop::NodeVarType>({ add, 0 }, { carve, 0 });

    auto foreach_end = std::make_shared<sop::node::ForeachPrimEnd>();
    eval.AddNode(foreach_end);

    hdiop::make_connecting<sop::NodeVarType>({ carve, 0 }, { foreach_end, 0 });

    auto dst_box = std::make_shared<sop::node::Box>();
    dst_box->SetSize({ 0.1f, 0.1f, 0.1f });
    eval.AddNode(dst_box);

    auto copy = std::make_shared<sop::node::CopyToPoints>();
    eval.AddNode(copy);

    eval.Connect({ dst_box, 0 },     { copy, sop::node::CopyToPoints::IDX_SRC_PRIM });
    eval.Connect({ foreach_end, 0 }, { copy, sop::node::CopyToPoints::IDX_TARGET_POS });

    eval.Update();

    test::check_points_num(copy, 64);
}

TEST_CASE("merge")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    const sm::vec3 size0(1, 2, 3);
    box0->SetSize(size0);
    const sm::vec3 pos0(21, 22, 23);
    box0->SetCenter(pos0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<sop::node::Box>();
    const sm::vec3 size1(11, 12, 13);
    box1->SetSize(size1);
    const sm::vec3 pos1(121, 122, 123);
    box1->SetCenter(pos1);
    eval.AddNode(box1);

    auto box2 = std::make_shared<sop::node::Box>();
    const sm::vec3 size2(21, 22, 23);
    box2->SetSize(size2);
    const sm::vec3 pos2(221, 222, 223);
    box2->SetCenter(pos2);
    eval.AddNode(box2);

    auto merge = std::make_shared<sop::node::Merge>();
    merge->AddInputPorts(3 - merge->GetImports().size());
    eval.AddNode(merge);

    eval.Connect({ box0, 0 }, { merge, 0 });
    eval.Connect({ box1, 0 }, { merge, 1 });
    eval.Connect({ box2, 0 }, { merge, 2 });

    eval.Update();

    test::check_points_num(merge, 24);
    test::check_faces_num(merge, 18);

    sm::vec3 aabb_min, aabb_max;
    aabb_min.x = std::min(pos0.x - size0.x * 0.5f, std::min(pos1.x - size1.x * 0.5f, pos2.x - size2.x * 0.5f));
    aabb_min.y = std::min(pos0.y - size0.y * 0.5f, std::min(pos1.y - size1.y * 0.5f, pos2.y - size2.y * 0.5f));
    aabb_min.z = std::min(pos0.z - size0.z * 0.5f, std::min(pos1.z - size1.z * 0.5f, pos2.z - size2.z * 0.5f));
    aabb_max.x = std::max(pos0.x + size0.x * 0.5f, std::max(pos1.x + size1.x * 0.5f, pos2.x + size2.x * 0.5f));
    aabb_max.y = std::max(pos0.y + size0.y * 0.5f, std::max(pos1.y + size1.y * 0.5f, pos2.y + size2.y * 0.5f));
    aabb_max.z = std::max(pos0.z + size0.z * 0.5f, std::max(pos1.z + size1.z * 0.5f, pos2.z + size2.z * 0.5f));
    test::check_aabb(merge, aabb_min, aabb_max);
}

TEST_CASE("merge 2")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    eval.AddNode(box0);

    auto box1 = std::make_shared<sop::node::Box>();
    eval.AddNode(box1);

    auto merge = std::make_shared<sop::node::Merge>();
    eval.AddNode(merge);

    eval.Connect({ box0, 0 }, { merge, sop::node::Merge::IDX_SRC_A });
    eval.Connect({ box1, 0 }, { merge, sop::node::Merge::IDX_SRC_B });

    SECTION("overlap")
    {
        eval.Update();

        test::check_faces_num(merge, 12);
        test::check_halfedge_faces_num(merge, 12);
    }

    SECTION("separate")
    {
        box1->SetCenter({ 0, 2, 0 });

        eval.Update();

        test::check_faces_num(merge, 12);
        test::check_halfedge_faces_num(merge, 12);
    }

    SECTION("connect")
    {
        box1->SetCenter({ 0, 1, 0 });

        eval.Update();

        test::check_faces_num(merge, 12);
        test::check_halfedge_faces_num(merge, 12);
    }
}

TEST_CASE("split")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto group = std::make_shared<sop::node::GroupCreate>();
    group->SetGroupName("top");
    group->SetGroupType(sop::GroupType::Primitives);
    group->EnableKeepByNormals(sm::vec3(0, 1, 0), 10);
    eval.AddNode(group);

    eval.Connect({ box, 0 }, { group, 0 });

    auto split = std::make_shared<sop::node::Split>();
    split->SetGroupName("top");
    eval.AddNode(split);

    eval.Connect({ group, 0 }, { split, 0 });

    auto null0 = std::make_shared<sop::node::Null>();
    auto null1 = std::make_shared<sop::node::Null>();
    eval.AddNode(null0);
    eval.AddNode(null1);
    eval.Connect({ split, sop::node::Split::IDX_OUT_0 }, { null0, 0 });
    eval.Connect({ split, sop::node::Split::IDX_OUT_1 }, { null1, 0 });

    eval.Update();

    test::check_faces_num(split, 1);

    test::check_faces_num(null0, 1);
    test::check_faces_num(null1, 5);
}

TEST_CASE("switch")
{
    test::init();

    sop::Evaluator eval;

    auto box0 = std::make_shared<sop::node::Box>();
    const sm::vec3 size0(1, 2, 3);
    box0->SetSize(size0);
    const sm::vec3 pos0(21, 22, 23);
    box0->SetCenter(pos0);
    eval.AddNode(box0);

    auto box1 = std::make_shared<sop::node::Box>();
    const sm::vec3 size1(11, 12, 13);
    box1->SetSize(size1);
    const sm::vec3 pos1(121, 122, 123);
    box1->SetCenter(pos1);
    eval.AddNode(box1);

    auto box2 = std::make_shared<sop::node::Box>();
    const sm::vec3 size2(21, 22, 23);
    box2->SetSize(size2);
    const sm::vec3 pos2(221, 222, 223);
    box2->SetCenter(pos2);
    eval.AddNode(box2);

    auto switch0 = std::make_shared<sop::node::Switch>();
    switch0->AddInputPorts(3 - switch0->GetImports().size());
    eval.AddNode(switch0);

    eval.Connect({ box0, 0 }, { switch0, 0 });
    eval.Connect({ box1, 0 }, { switch0, 1 });
    eval.Connect({ box2, 0 }, { switch0, 2 });

    SECTION("init sel 0")
    {
        eval.Update();

        test::check_aabb(switch0, pos0 - size0 * 0.5f, pos0 + size0 * 0.5f);
    }

    SECTION("sel 1")
    {
        switch0->SetSelected(1);

        eval.Update();

        test::check_aabb(switch0, pos1 - size1 * 0.5f, pos1 + size1 * 0.5f);
    }
}