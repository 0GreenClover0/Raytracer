#include "Interval.h"

Interval const Interval::empty = Interval(+AK::INFINITY_F, -AK::INFINITY_F);
Interval const Interval::whole = Interval(-AK::INFINITY_F, +AK::INFINITY_F);

Interval::Interval() : min(+AK::INFINITY_F), max(-AK::INFINITY_F)
{
}

Interval::Interval(float const min, float const max) : min(min), max(max)
{
}

float Interval::size() const
{
    return max - min;
}

bool Interval::contains(float const x) const
{
    return min <= x && x <= max;
}

bool Interval::surrounds(float const x) const
{
    return min < x && x < max;
}
