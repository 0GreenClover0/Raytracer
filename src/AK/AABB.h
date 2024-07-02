#pragma once

#include "Interval.h"
#include "Renderer/Ray.h"
#include "Types.h"

struct AABB
{
    Interval x = {};
    Interval y = {};
    Interval z = {};

    AABB() = default;

    AABB(Interval const& x, Interval const& y, Interval const& z);

    AABB(glm::vec3 const& a, glm::vec3 const& b);

    AABB(AABB const& box0, AABB const& box1);

    [[nodiscard]] Interval const& axis_interval(i32 const n) const;

    [[nodiscard]] bool hit(Ray const& ray, Interval ray_t) const;
};
