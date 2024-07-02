#pragma once

#include "Interval.h"
#include "Renderer/Ray.h"

struct AABB
{
    Interval x;
    Interval y;
    Interval z;

    AABB() = default;

    AABB(Interval const& x, Interval const& y, Interval const& z);

    AABB(glm::vec3 const& a, glm::vec3 const& b);

    [[nodiscard]] Interval const& axis_interval(i32 const n) const;

    [[nodiscard]] bool hit(Ray const& ray, Interval ray_t) const;
};
