#include "Interval.h"

Interval const Interval::empty = Interval(+AK::INFINITY_F, -AK::INFINITY_F);
Interval const Interval::whole = Interval(-AK::INFINITY_F, +AK::INFINITY_F);

Interval::Interval() : min(+AK::INFINITY_F), max(-AK::INFINITY_F)
{
}

Interval::Interval(float const min, float const max) : min(min), max(max)
{
}

Interval::Interval(Interval const& a, Interval const& b)
{
    min = a.min <= b.min ? a.min : b.min;
    max = a.max >= b.max ? a.max : b.max;
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

Interval Interval::expand(float const delta) const
{
    float const padding = delta / 2.0f;
    return Interval(min - padding, max + padding);
}
