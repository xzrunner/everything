#ifndef PARM_FILEPATH
#error "You must define PARM_FILEPATH macro before include this file"
#endif

#define Vector3 sm::vec3

// names
public:
static const constexpr char* const ParmNames[] = {
#define PARAM_INFO(id, type, name, member, default_val) \
    #name,
#include PARM_FILEPATH
#undef  PARAM_INFO
};

// type enums
public:
enum class Parm
{
#define PARAM_INFO(id, type, name, member, default_val) \
    id,
#include PARM_FILEPATH
#undef  PARAM_INFO
};

// setter & getter
public:
#define PARAM_INFO(id, type, name, member, default_val) \
    void Set##id(const type& name)                      \
    {                                                   \
        if (member != name) {                           \
            member = name;                              \
            SetDirty(true);                             \
        }                                               \
    }                                                   \
    auto& Get##id() const                               \
    {                                                   \
        return member;                                  \
    }
#include PARM_FILEPATH
#undef  PARAM_INFO

// member variables
private:
#define PARAM_INFO(id, type, name, member, default_val) \
    type member = type##default_val;
#include PARM_FILEPATH
#undef  PARAM_INFO

#undef Vector3
