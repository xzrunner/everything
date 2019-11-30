#pragma once

#include "sop/ParmType.h"
#include "sop/GeoAttrDefine.h"

#include <SM_Vector.h>

#include <vector>
#include <memory>

namespace sop
{

class ParmList
{
public:
    ParmList() {}
    ParmList(const ParmList& p);
    ParmList(GeoAttr attr);
    ParmList(const std::string& name, ParmType type);

    ParmList& operator = (const ParmList& list);

    virtual std::shared_ptr<ParmList> Clone(bool clone_items = true) const = 0;

    virtual void CopyFrom(size_t dst_idx, const ParmList& src, size_t src_idx) = 0;
    virtual void Append(const ParmList& list) = 0;

    virtual void Clear() = 0;
    virtual void Resize(size_t num) = 0;

    virtual size_t Size() const = 0;

    auto& GetAttr() const { return m_attr; }

    auto& GetName() const { return m_name; }
    auto& GetType() const { return m_type; }

protected:
    GeoAttr     m_attr = GEO_ATTR_UNKNOWN;

    std::string m_name;
    ParmType    m_type = ParmType::Boolean;

}; // ParmList

template <typename T>
class ParmListImpl : public ParmList
{
public:
    ParmListImpl(GeoAttr attr, std::vector<T> items)
        : ParmList(attr)
        , m_items(items)
    {
    }
    ParmListImpl(const std::string& name, ParmType type, std::vector<T> items)
        : ParmList(name, type)
        , m_items(items)
    {
    }

    virtual std::shared_ptr<ParmList> Clone(bool clone_items = true) const override
    {
        if (clone_items) {
            return std::make_shared<ParmListImpl<T>>(m_name, m_type, m_items);
        } else {
            return std::make_shared<ParmListImpl<T>>(m_name, m_type, std::vector<T>());
        }
    }

    virtual void CopyFrom(size_t dst_idx, const ParmList& src, size_t src_idx) override
    {
        if (GetType() == src.GetType()) {
            m_items[dst_idx] = static_cast<const ParmListImpl<T>&>(src).m_items[src_idx];
        }
    }
    virtual void Append(const ParmList& list) override
    {
        if (GetType() == list.GetType())
        {
            auto& src = static_cast<const ParmListImpl<T>&>(list).m_items;
            std::copy(src.begin(), src.end(), std::back_inserter(m_items));
        }
        else
        {
            m_items.resize(m_items.size() + list.Size());
        }
    }

    virtual void Clear() override { m_items.clear(); }
    virtual void Resize(size_t num) override { m_items.resize(num); }

    virtual size_t Size() const override { return m_items.size(); }

    auto& GetAllItems() const { return m_items; }

private:
    std::vector<T> m_items;

}; // ParmListImpl

typedef ParmListImpl<bool> ParmBoolList;

typedef ParmListImpl<int>       ParmIntList;
typedef ParmListImpl<sm::ivec2> ParmInt2List;
typedef ParmListImpl<sm::ivec3> ParmInt3List;
typedef ParmListImpl<sm::ivec4> ParmInt4List;

typedef ParmListImpl<float>    ParmFltList;
typedef ParmListImpl<sm::vec2> ParmFlt2List;
typedef ParmListImpl<sm::vec3> ParmFlt3List;
typedef ParmListImpl<sm::vec4> ParmFlt4List;

typedef ParmListImpl<std::string> ParmStrList;

}