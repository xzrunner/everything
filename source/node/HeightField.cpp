#include "sop/node/HeightField.h"
#include "sop/Volume.h"

namespace sop
{
namespace node
{

void HeightField::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_volume = std::make_shared<Volume>(sm::ivec3(m_sizex, 1, m_sizey));
}

}
}