#include "everything/VarValue.h"

namespace evt
{

VarValue::VarValue(bool b)
    : b(b)
{
}

VarValue::VarValue(int  i)
    : i(i)
{
}

VarValue::VarValue(float f)
    : f(f)
{
}

VarValue::VarValue(double d)
    : d(d)
{
}

VarValue::VarValue(const std::string& str)
{
    auto buf = new char[str.size() + 1];
    std::strcpy(buf, str.c_str());
    p = buf;
}

VarValue::VarValue(const sm::vec3& v3)
{
    auto buf = new float[3];
    std::memcpy(buf, v3.xyz, sizeof(float) * 3);
    p = buf;
}

}