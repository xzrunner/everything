#include "utility.h"

#include <everything/TreeContext.h>
#include <everything/Evaluator.h>
#include <everything/GeometryImpl.h>
#include <everything/GeoAttrName.h>

#include <everything/node/Carve.h>

#include <everything/node/Line.h>
#include <everything/node/Color.h>

#include <catch/catch.hpp>

TEST_CASE("carve")
{
    test::init();

    evt::Evaluator eval;

    auto line = std::make_shared<evt::node::Line>();

    sm::vec3 ori(0, 0, 0);
    sm::vec3 dir(0, 1, 0);
    float len = 10.0f;
    size_t num = 3;

    line->SetOrigin(ori);
    line->SetDirection(dir);
    line->SetLength(len);
    line->SetPoints(num);

    eval.AddNode(line);

    auto color = std::make_shared<evt::node::Color>();
    color->SetAttrAddTo(evt::GeoAttrType::Point);
    color->SetColor({ 0.3f, 0.4f, 0.5f });
    eval.AddNode(color);

    eval.Connect({ line, 0 }, { color, 0 });

    auto carve = std::make_shared<evt::node::Carve>();
    eval.AddNode(carve);

    eval.Connect({ color, 0 }, { carve, 0 });

    SECTION("first u")
    {
        carve->SetFirstU(0.2f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 2, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 10, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.5f));
    }

    SECTION("second u")
    {
        carve->SetSecondU(0.7f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 0, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 7, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.0f));
    }

    SECTION("first and second u")
    {
        carve->SetFirstU(0.3f);
        carve->SetSecondU(0.6f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 3, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 6, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.0f));
    }

    SECTION("first and second u inv")
    {
        carve->SetFirstU(0.6f);
        carve->SetSecondU(0.3f);

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 3, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 6, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 3);
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 1, evt::Variable(0.3f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_x, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 1, evt::Variable(0.4f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 2, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 0, evt::Variable(0.0f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 1, evt::Variable(0.5f));
        test::check_attr_value(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_z, 2, evt::Variable(0.0f));
    }

    SECTION("expr fit")
    {
        auto& props = const_cast<evt::NodePropsMgr&>(carve->GetProps());
        REQUIRE(props.Size() == evt::node::Carve::MAX_BUILD_IN_PROP);

        props.SetExpr(evt::node::Carve::FIRST_U,  "fit(3, 0, 10, 0, 1)");
        props.SetExpr(evt::node::Carve::SECOND_U, "1 - ch(\"domainu1\")");

        eval.Update();

        test::check_points_num(carve, 3);
        test::check_point(carve, 0, sm::vec3(0, 3, 0));
        test::check_point(carve, 1, sm::vec3(0, 5, 0));
        test::check_point(carve, 2, sm::vec3(0, 7, 0));

        test::check_attr_count(carve, evt::GeoAttrType::Point, evt::GeoAttrName::col_y, 3);
    }
}