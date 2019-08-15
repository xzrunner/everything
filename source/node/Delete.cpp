#include "everything/node/Delete.h"

namespace evt
{
namespace node
{

void Delete::Execute(TreeContext& ctx)
{
}

void Delete::SetDelNonSelected(bool del_non_selected)
{
    if (m_delete_non_selected == del_non_selected) {
        return;
    }

    m_delete_non_selected = del_non_selected;

    SetDirty(true);
}

void Delete::SetEntityType(EntityType type)
{
    if (m_entity_type == type) {
        return;
    }

    m_entity_type = type;

    SetDirty(true);
}

void Delete::SetFilterExp(const std::string& exp)
{
    if (m_filter_exp == exp) {
        return;
    }

    m_filter_exp = exp;

    SetDirty(true);
}

}
}