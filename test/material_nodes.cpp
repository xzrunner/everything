#include "utility.h"

#include <sop/Evaluator.h>

#include <sop/node/UVTransform.h>
#include <sop/node/UVUnwrap.h>

#include <sop/node/Box.h>

#include <catch/catch.hpp>

TEST_CASE("uv transform")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto uv_unwrap = std::make_shared<sop::node::UVUnwrap>();
    eval.AddNode(uv_unwrap);

    eval.Connect({ box, 0 }, { uv_unwrap, 0 });

    auto uv_transform = std::make_shared<sop::node::UVTransform>();
    uv_transform->SetScale(sm::vec3(0.5f, 0.5f, 0.5f));
    eval.AddNode(uv_transform);

    eval.Connect({ uv_unwrap, 0 }, { uv_transform, 0 });

    eval.Update();

    test::check_attr_value(uv_unwrap, sop::GeoAttrClass::Vertex,
        sop::GeoAttrNames[sop::GEO_ATTR_UV], 1, sop::Variable(sm::vec3(1, 0, 0)));
    test::check_attr_value(uv_transform, sop::GeoAttrClass::Vertex,
        sop::GeoAttrNames[sop::GEO_ATTR_UV], 1, sop::Variable(sm::vec3(0.5f, 0, 0)));
}

TEST_CASE("uv unwrap")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto uv_unwrap = std::make_shared<sop::node::UVUnwrap>();
    eval.AddNode(uv_unwrap);

    eval.Connect({ box, 0 }, { uv_unwrap, 0 });

    eval.Update();
}
