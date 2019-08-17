#include "everything/Expression.h"

namespace evt
{

void Expression::Reset(const std::string& str)
{
    size_t pos = 0;
    if ((pos = str.find("==")) != std::string::npos)
    {
        m_op = Operator::Equal;
        pos += 2;
    }
    else if ((pos = str.find("!=")) != std::string::npos)
    {
        m_op = Operator::NotEqual;
        pos += 2;
    }
    else if ((pos = str.find("<")) != std::string::npos)
    {
        m_op = Operator::Less;
        pos += 1;
    }
    else if ((pos = str.find("<=")) != std::string::npos)
    {
        m_op = Operator::LessEqual;
        pos += 2;
    }
    else if ((pos = str.find(">")) != std::string::npos)
    {
        m_op = Operator::Greater;
        pos += 1;
    }
    else if ((pos = str.find(">=")) != std::string::npos)
    {
        m_op = Operator::GreaterEqual;
        pos += 2;
    }
    else
    {
        assert(0);
    }

    assert(strncmp(str.c_str(), "@P.", 3) == 0);
    if (str[3] == 'x') {
        m_left = OpLeft::X;
    } else if (str[3] == 'y') {
        m_left = OpLeft::Y;
    } else if (str[3] == 'z') {
        m_left = OpLeft::Z;
    } else {
        assert(0);
    }

    m_right = static_cast<float>(std::atof(&str[pos]));
}

bool Expression::Calc(const sm::vec3& p) const
{
    bool ret = false;

    float left = 0.0f;
    switch (m_left)
    {
    case OpLeft::X:
        left = p.x;
        break;
    case OpLeft::Y:
        left = p.y;
        break;
    case OpLeft::Z:
        left = p.z;
        break;
    default:
        assert(0);
    }

    switch (m_op)
    {
    case Operator::Equal:
        ret = left == m_right;
        break;
    case Operator::NotEqual:
        ret = left != m_right;
        break;
    case Operator::Less:
        ret = left < m_right;
        break;
    case Operator::LessEqual:
        ret = left <= m_right;
        break;
    case Operator::Greater:
        ret = left > m_right;
        break;
    case Operator::GreaterEqual:
        ret = left >= m_right;
        break;
    default:
        assert(0);
    }

    return ret;
}

}