#pragma once

#include <vector>

namespace sop
{

class VarBuffer
{
public:
    ~VarBuffer() {
        for (auto& v : m_vars) {
            delete v;
        }
    }

    template<typename T>
    T* Clone(const T& v) {
        auto new_v = new T(v);
        m_vars.push_back(new_v);
        return new_v;
    }

private:
    std::vector<void*> m_vars;

}; // VarBuffer

}