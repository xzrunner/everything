#pragma once

#include <SM_Vector.h>

#include <string>

namespace sop
{

union VarValue
{
    VarValue() {}
    explicit VarValue(bool b);
    explicit VarValue(int  i);
    explicit VarValue(float f);
    explicit VarValue(double d);
    explicit VarValue(const std::string& str);
    explicit VarValue(const sm::vec3& v3);

    bool   b;
    int    i;
    float  f;
    double d;
    const void* p = nullptr;
};

}