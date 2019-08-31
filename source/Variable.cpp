#include "everything/Variable.h"

namespace evt
{

Variable::Variable(bool b)
    : type(VariableType::Bool)
    , b(b)
{
}

Variable::Variable(int  i)
    : type(VariableType::Int)
    , i(i)
{
}

Variable::Variable(float f)
    : type(VariableType::Float)
    , f(f)
{
}

Variable::Variable(double d)
    : type(VariableType::Double)
    , d(d)
{
}

Variable::Variable(const std::string& str)
    : type(VariableType::String)
{
    auto buf = new char[str.size() + 1];
    std::strcpy(buf, str.c_str());
    p = buf;
}

Variable::Variable(const sm::vec3& v3)
    : type(VariableType::Float3)
{
    auto buf = new float[3];
    std::memcpy(buf, v3.xyz, sizeof(float) * 3);
    p = buf;
}

Variable::Variable(const Variable& var)
    : type(var.type)
{
    switch (type)
    {
    case VariableType::Float3:
    {
        auto src = static_cast<const float*>(var.p);
        auto buf = new float[3];
        memcpy(buf, var.p, sizeof(float) * 3);
        p = buf;
    }
        break;
    case VariableType::String:
    {
        auto src = static_cast<const char*>(var.p);
        auto buf = new char[strlen(src) + 1];
        std::strcpy(buf, src);
        p = buf;
    }
        break;
    default:
        memcpy(this, &var, sizeof(var));
    }
}

Variable& Variable::operator = (const Variable& var)
{
    switch (type)
    {
    case VariableType::Float3:
        delete[] static_cast<const float*>(p);
        break;
    case VariableType::String:
        delete[] static_cast<const char*>(p);
        break;
    }

    type = var.type;
    switch (type)
    {
    case VariableType::Float3:
    {
        auto src = static_cast<const float*>(var.p);
        auto buf = new float[3];
        memcpy(buf, var.p, sizeof(float) * 3);
        p = buf;
    }
        break;
    case VariableType::String:
    {
        auto src = static_cast<const char*>(var.p);
        auto buf = new char[strlen(src) + 1];
        std::strcpy(buf, src);
        p = buf;
    }
        break;
    default:
        memcpy(this, &var, sizeof(var));
    }
    return *this;
}

Variable::~Variable()
{
    switch (type)
    {
    case VariableType::Float3:
        delete[] static_cast<const float*>(p);
        break;
    case VariableType::String:
        delete[] static_cast<const char*>(p);
        break;
    }
}

bool Variable::operator == (const Variable& var) const
{
    if (type != var.type) {
        return false;
    }
    switch (type)
    {
    case VariableType::Invalid:
        return true;
    case VariableType::Bool:
        return b == var.b;
    case VariableType::Int:
        return i == var.i;
    case VariableType::Float:
        return f == var.f;
    case VariableType::Float3:
    {
        auto v0 = static_cast<const float*>(p);
        auto v1 = static_cast<const float*>(var.p);
        return v0[0] == v1[0]
            && v0[1] == v1[1]
            && v0[2] == v1[2];
    }
    case VariableType::Double:
        return d == var.d;
    case VariableType::String:
        return strcmp(static_cast<const char*>(p),
            static_cast<const char*>(var.p)) == 0;
    default:
        assert(0);
        return false;
    }
}

}