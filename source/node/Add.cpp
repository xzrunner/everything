#include "everything/node/Add.h"

namespace evt
{
namespace node
{

void Add::Execute(TreeContext& ctx)
{
}

void Add::SetPoints(const std::vector<sm::vec3>& points)
{
    if (m_points == points) {
        return;
    }

    m_points = points;

    SetDirty(true);
}

}
}