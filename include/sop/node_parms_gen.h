#ifndef PARM_FILEPATH
#error "You must define PARM_FILEPATH macro before include this file"
#endif

#define Boolean bool
#define Integer int
#define Float   float
#define Vector3 sm::vec3
#define Vector4 sm::vec4
#define String  std::string
#define Array   std::vector

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
    void Set##id(const type& _##name)                   \
    {                                                   \
        if (member != _##name) {                        \
            member = _##name;                           \
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

#undef Boolean
#undef Integer
#undef Float
#undef Vector3
#undef Vector4
#undef String
#undef Array