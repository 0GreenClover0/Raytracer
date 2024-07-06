#pragma once

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

inline Interval operator+(Interval const& interval, float const displacement)
{
    return Interval(interval.min + displacement, interval.max + displacement);
}

inline Interval operator+(float const displacement, Interval const& interval)
{
    return interval + displacement;
}
