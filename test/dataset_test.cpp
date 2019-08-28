#include "utility.h"

#include <everything/node/Box.h>
#include <everything/TreeContext.h>

#include <catch/catch.hpp>

TEST_CASE("node prop")
{
    test::init();

    auto box = std::make_shared<evt::node::Box>();
    box->Execute(evt::TreeContext());

    box->AddProperty("prop0", evt::Variable(1.1f));
    box->AddProperty("prop1", evt::Variable(2));
    box->AddProperty("prop2", evt::Variable(sm::vec3(4, 5, 6)));
    box->AddProperty("prop3", evt::Variable(true));
    box->AddProperty("prop4", evt::Variable("zz"));

    test::check_prop(box, "prop0", evt::Variable(1.1f));
    test::check_prop(box, "prop1", evt::Variable(2));
    test::check_prop(box, "prop2", evt::Variable(sm::vec3(4, 5, 6)));
    test::check_prop(box, "prop3", evt::Variable(true));
    test::check_prop(box, "prop4", evt::Variable("zz"));

    box->RemoveProperty("prop2");
    test::check_prop(box, "prop2", evt::Variable());

    box->ClearProperty();
    test::check_prop(box, "prop3", evt::Variable());
}
