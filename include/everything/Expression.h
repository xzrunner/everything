#pragma once

#include <SM_Vector.h>

#include <string>

namespace evt
{

class Expression
{
public:
    Expression() {}

    void Reset(const std::string& str);

    bool Calc(const sm::vec3& p) const;

private:
    enum class OpLeft
    {
        X,
        Y,
        Z,
    };

    enum class Operator
    {
        Equal,
        NotEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
    };

private:
    OpLeft   m_left;
    Operator m_op;
    float    m_right = 0.0f;

}; // Expression

}