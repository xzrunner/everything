#pragma once

namespace sop
{

struct RampWidgets
{
    enum COMP_ID
    {
        COMP_VALUE,
        COMP_INTERP,
        COMP_POS
    };

    static const constexpr char* const COMP_NAMES[] = {
        { "value" },
        { "interp" },
        { "pos" }
    };

    bool operator == (const RampWidgets& ramp) const {
        return value == ramp.value
            && interp == ramp.interp
            && pos == ramp.pos;
    }

    bool operator != (const RampWidgets& ramp) const {
        return !(operator == (ramp));
    }

    float       value  = 0;
    std::string interp;
    float       pos    = 0;
};

}