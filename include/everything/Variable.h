#pragma once

#include <SM_Vector.h>

#include <string>

namespace evt
{

enum class VariableType
{
    Invalid = 0,

    Bool,
    Int,
    Float,
    Float3,
    Double,
    String,
};

class Variable
{
public:
    Variable() : type(VariableType::Invalid) {}
    explicit Variable(bool b);
    explicit Variable(int  i);
    explicit Variable(float f);
    explicit Variable(double d);
    explicit Variable(const std::string& str);
    explicit Variable(const sm::vec3& v3);
    Variable(const Variable& var);
    Variable& operator = (const Variable& var);
    ~Variable();

    bool operator == (const Variable& var) const;

    VariableType type = VariableType::Invalid;

    union
    {
        bool   b;
        int    i;
        float  f;
        double d;
        const void* p = nullptr;
    };

}; // Variable

}