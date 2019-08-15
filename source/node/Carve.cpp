#include "everything/node/Carve.h"

namespace evt
{
namespace node
{

void Carve::Execute(TreeContext& ctx)
{
}

void Carve::SetFirstU(float u)
{
    if (u == m_first_u) {
        return;
    }

    m_first_u = u;

    SetDirty(true);
}

void Carve::SetSecondU(float u)
{
    if (u == m_second_u) {
        return;
    }

    m_second_u = u;

    SetDirty(true);
}

void Carve::SetFirstV(float v)
{
    if (v == m_first_v) {
        return;
    }

    m_first_v = v;

    SetDirty(true);
}

void Carve::SetSecondV(float v)
{
    if (v == m_second_v) {
        return;
    }

    m_second_v = v;

    SetDirty(true);
}

}
}