#pragma once

#include "Math.h"

class Interval
{
public:
    float min;
    float max;

    Interval();
    Interval(float const min, float const max);
    Interval(Interval const& a, Interval const& b);

    [[nodiscard]] float size() const;
    [[nodiscard]] bool contains(float const x) const;
    [[nodiscard]] bool surrounds(float const x) const;

    [[nodiscard]] Interval expand(float const delta) const;

    static Interval const empty;
    static Interval const whole;
};
