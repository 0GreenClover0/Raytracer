#include "AABB.h"

AABB::AABB(Interval const& x, Interval const& y, Interval const& z) : x(x), y(y), z(z)
{
}

AABB::AABB(glm::vec3 const& a, glm::vec3 const& b)
{
    x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
    y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
    z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
}

Interval const& AABB::axis_interval(i32 const n) const
{
    if (n == 1)
    {
        return y;
    }

    if (n == 2)
    {
        return z;
    }

    return x;
}

bool AABB::hit(Ray const& ray, Interval ray_t) const
{
    glm::vec3 const& ray_origin = ray.origin();
    glm::vec3 const& ray_direction = ray.direction();

    for (i32 axis = 0; axis < 3; ++axis)
    {
        Interval const& ax = axis_interval(axis);
        float const adinv = 1.0f / ray_direction[axis];

        float const t0 = (ax.min - ray_origin[axis]) * adinv;
        float const t1 = (ax.max - ray_origin[axis]) * adinv;

        if (t0 < t1)
        {
            if (t0 > ray_t.min)
            {
                ray_t.min = t0;
            }

            if (t1 < ray_t.max)
            {
                ray_t.max = t1;
            }
        }
        else
        {
            if (t1 > ray_t.min)
            {
                ray_t.min = t1;
            }

            if (t0 < ray_t.max)
            {
                ray_t.max = t0;
            }
        }

        if (ray_t.max <= ray_t.min)
        {
            return false;
        }
    }

    return true;
}
