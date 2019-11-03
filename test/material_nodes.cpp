#include "utility.h"

#include <sop/Evaluator.h>

#include <sop/node/UVUnwrap.h>

#include <sop/node/Box.h>

#include <catch/catch.hpp>

TEST_CASE("uv unwrap")
{
    test::init();

    sop::Evaluator eval;

    auto box = std::make_shared<sop::node::Box>();
    eval.AddNode(box);

    auto uvunwrap = std::make_shared<sop::node::UVUnwrap>();
    eval.AddNode(uvunwrap);

    eval.Connect({ box, 0 }, { uvunwrap, 0 });

    eval.Update();
}
