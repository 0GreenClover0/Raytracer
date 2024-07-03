#include "BVHNode.h"

#include "AK/AK.h"

#include <algorithm>

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& hittables, size_t const start, size_t const end)
{
    // Build the bounding box of the span of source hittables.
    m_bbox = AABB::empty;

    for (size_t index = start; index < end; ++index)
    {
        m_bbox = AABB(m_bbox, hittables[index]->bounding_box());
    }

    i32 const axis = m_bbox.longest_axis();

    auto const comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

    size_t const hittables_span = end - start;

    if (hittables_span == 1)
    {
        m_hittable_left = m_hittable_right = hittables[start];
    }
    else if (hittables_span == 2)
    {
        m_hittable_left = hittables[start];
        m_hittable_right = hittables[start + 1];
    }
    else
    {
        std::sort(hittables.begin() + static_cast<i64>(start), hittables.begin() + static_cast<i64>(end), comparator);

        u64 const mid = start + hittables_span / 2;

        m_left = std::make_shared<BVHNode>(hittables, start, mid);
        m_right = std::make_shared<BVHNode>(hittables, mid, end);
    }
}

bool BVHNode::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    if (!m_bbox.hit(ray, ray_t))
    {
        return false;
    }

    // Only check for m_left, because m_right should be null when the m_left is, and not null when the m_left is not.
    if (m_left == nullptr)
    {
        bool const hit_left = m_hittable_left->hit(ray, ray_t, hit_record);
        bool const hit_right = m_hittable_right->hit(ray, Interval(ray_t.min, hit_left ? hit_record.t : ray_t.max), hit_record);
        return hit_left || hit_right;
    }

    bool const hit_left = m_left->hit(ray, ray_t, hit_record);
    bool const hit_right = m_right->hit(ray, Interval(ray_t.min, hit_left ? hit_record.t : ray_t.max), hit_record);

    return hit_left || hit_right;
}

AABB BVHNode::bounding_box() const
{
    return m_bbox;
}

bool BVHNode::box_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b, i32 const axis_index)
{
    return a->bounding_box().axis_interval(axis_index).min < b->bounding_box().axis_interval(axis_index).min;
}

bool BVHNode::box_x_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b)
{
    return box_compare(a, b, 0);
}

bool BVHNode::box_y_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b)
{
    return box_compare(a, b, 1);
}

bool BVHNode::box_z_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b)
{
    return box_compare(a, b, 2);
}
