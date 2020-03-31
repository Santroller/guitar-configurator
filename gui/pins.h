#pragma once
#include <QObject>
#include "submodules/Ardwiino/src/shared/config/config.h"
#define PIN_PROP(f) Q_PROPERTY(uint8_t p_##f MEMBER f)
class Pins: pins_t
{
    Q_GADGET
    PIN_PROP(up)
    PIN_PROP(down)
    PIN_PROP(left)
    PIN_PROP(right)
    PIN_PROP(start)
    PIN_PROP(back)
    PIN_PROP(left_stick)
    PIN_PROP(right_stick)
    PIN_PROP(LB)
    PIN_PROP(RB)
    PIN_PROP(home)
    PIN_PROP(unused)
    PIN_PROP(a)
    PIN_PROP(b)
    PIN_PROP(x)
    PIN_PROP(y)
    PIN_PROP(lt)
    PIN_PROP(rt)
    PIN_PROP(l_x)
    PIN_PROP(l_y)
    PIN_PROP(r_x)
    PIN_PROP(r_y)
    public:
    inline bool operator==(const Pins &other) const
    {
        // compare members
        return up == other.up && down == other.down;
    }

    inline bool operator!=(Pins const &other) const
    {
        return !(*this == other);
    }
};
Q_DECLARE_METATYPE(Pins)
