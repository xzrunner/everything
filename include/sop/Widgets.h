#pragma once

namespace sop
{

enum class InterpType
{
    Constant,
    Linear,
    Catmull_Rom,
    MonotoneCubic,
    Bezier,
    B_Spline,
    Hermite,
};

struct RampFloat
{
    enum COMP_ID
    {
        COMP_VALUE,
        COMP_INTERP,
        COMP_POS,

        COMP_MAX_COUNT
    };

    static const constexpr char* const COMP_NAMES[] = {
        { "value" },
        { "interp" },
        { "pos" }
    };

    bool operator == (const RampFloat& ramp) const {
        return value == ramp.value
            && interp == ramp.interp
            && pos == ramp.pos;
    }

    bool operator != (const RampFloat& ramp) const {
        return !(operator == (ramp));
    }

    float      value  = 0;
    InterpType interp = InterpType::Linear;
    float      pos    = 0;
};

struct RampColor
{
    enum COMP_ID
    {
        COMP_COLOR,
        COMP_INTERP,
        COMP_POS,

        COMP_MAX_COUNT
    };

    static const constexpr char* const COMP_NAMES[] = {
        { "c" },
        { "interp" },
        { "pos" }
    };

    bool operator == (const RampColor& ramp) const {
        return color == ramp.color
            && interp == ramp.interp
            && pos == ramp.pos;
    }

    bool operator != (const RampColor& ramp) const {
        return !(operator == (ramp));
    }

    sm::vec3   color  = sm::vec3(1.0f, 1.0f, 1.0f);
    InterpType interp = InterpType::Linear;
    float      pos    = 0;
};

}