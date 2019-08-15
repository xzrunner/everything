#include "everything/node/Sort.h"

namespace evt
{
namespace node
{

void Sort::Execute(TreeContext& ctx)
{
}

void Sort::SetKey(Key key)
{
    if (m_key == key) {
        return;
    }

    m_key = key;

    SetDirty(true);
}

}
}
