#include "sop/Volume.h"

namespace sop
{

Volume::Volume(const sm::ivec3& size)
    : m_size(size)
{
    m_values.resize(size.x * size.y * size.z, 0.0f);
}

}