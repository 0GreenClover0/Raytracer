#include "AABB.h"

AABB const AABB::empty = AABB(Interval::empty, Interval::empty, Interval::empty);
AABB const AABB::whole = AABB(Interval::whole, Interval::whole, Interval::whole);

AABB::AABB(Interval const& x, Interval const& y, Interval const& z) : x(x), y(y), z(z)
{
}

AABB::AABB(glm::vec3 const& a, glm::vec3 const& b)
{
    x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
    y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
    z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
}

AABB::AABB(AABB const& box0, AABB const& box1)
{
    x = Interval(box0.x, box1.x);
    y = Interval(box0.y, box1.y);
    z = Interval(box0.z, box1.z);
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

// Returns the index of the longest axis of the bounding box.
i32 AABB::longest_axis() const
{
    if (x.size() > y.size())
    {
        return x.size() > z.size() ? 0 : 2;
    }

    return y.size() > z.size() ? 1 : 2;
}
