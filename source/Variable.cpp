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

Variable::Variable(const sm::vec3& f3)
    : type(VariableType::Float3)
    , f3(f3)
{
}

Variable::Variable(const std::string& str)
    : type(VariableType::String)
{
    c = new char[str.size() + 1];
    std::strcpy(c, str.c_str());
}

Variable::Variable(const Variable& var)
    : type(var.type)
{
    if (type == VariableType::String) {
        c = new char[strlen(var.c) + 1];
        std::strcpy(c, var.c);
    } else {
        memcpy(this, &var, sizeof(var));
    }
}

Variable& Variable::operator = (const Variable& var)
{
    if (type == VariableType::String) {
        delete[] c;
        c = nullptr;
    }

    type = var.type;
    if (type == VariableType::String) {
        c = new char[strlen(var.c) + 1];
        std::strcpy(c, var.c);
    } else {
        memcpy(this, &var, sizeof(var));
    }
    return *this;
}

Variable::~Variable()
{
    if (type == VariableType::String) {
        delete[] c;
        c = nullptr;
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
        return f3 == var.f3;
    case VariableType::String:
        return strcmp(c, var.c) == 0;
    default:
        assert(0);
        return false;
    }
}

}