#include <sop/PyLoader.h>

#include <catch/catch.hpp>

TEST_CASE("run python")
{
    sop::PyLoader loader;
//    loader.RunFile("D:/OneDrive/work/houdini/data/house_test.txt");
//    loader.RunFile("D:/OneDrive/work/houdini/data/house_tmp.txt");
    loader.RunFile("D:/OneDrive/work/houdini/data/house.txt");
}