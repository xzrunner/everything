#pragma once

#include "sop/typedef.h"

#include <string>

#include <boost/python.hpp>

namespace sop
{
namespace py
{

class Keyframe;
class StringKeyframe;

class Parm
{
public:
    Parm(const std::string& name, const NodePtr& node)
        : m_name(name)
        , m_node(node)
    {
    }

    void SetKeyframe(const Keyframe& k) {}
    void SetKeyframe(const StringKeyframe& k) {}

    void Set(float val);
    void Set(const std::string& val);
    void Set(int val);

private:
    std::string m_name;

    NodePtr m_node = nullptr;

}; // Parm

class ParmTuple
{
public:
    ParmTuple(const std::string& name, const NodePtr& node)
        : m_name(name)
        , m_node(node)
    {
    }

    void SetAutoscope(const boost::python::tuple& bool_values) {}
    void Set(const boost::python::tuple& val);
    Parm GetItem(size_t i);

private:
    std::string m_name;

    NodePtr m_node = nullptr;

}; // ParmTuple

void BindParmValue();

}
}