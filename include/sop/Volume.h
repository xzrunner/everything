#pragma once

#include <SM_Vector.h>

#include <vector>

namespace sop
{

class Volume
{
public:
    Volume(const sm::ivec3& size);

    auto& GetSize() const { return m_size; }
    void  SetSize(const sm::ivec3& sz) { m_size = sz; }

    auto& GetValues() const { return m_values; }
    void  SetValues(const std::vector<float>& values) { m_values = values; }

private:
    sm::ivec3 m_size;

    std::vector<float> m_values;

}; // Volume

}