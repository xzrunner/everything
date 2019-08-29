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
    String,
};

class Variable
{
public:
    Variable() : type(VariableType::Invalid) {}
    explicit Variable(bool b);
    explicit Variable(int  i);
    explicit Variable(float f);
    explicit Variable(const sm::vec3& f3);
    explicit Variable(const std::string& str);
    Variable(const Variable& var);
    Variable& operator = (const Variable& var);
    ~Variable();

    bool operator == (const Variable& var) const;

    VariableType type;

    union
    {
        bool     b;
        int      i;
        float    f;
        sm::vec3 f3;
        char*    c = nullptr;
    };

}; // Variable

}