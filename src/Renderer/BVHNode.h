#pragma once

#include "Hittable.h"

class BVHNode
{
public:
    BVHNode(std::vector<std::shared_ptr<Hittable>>& hittables, size_t const start, size_t const end);

    [[nodiscard]] bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const;

    [[nodiscard]] AABB bounding_box() const;

private:
    static bool box_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b, i32 const axis_index);
    static bool box_x_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b);
    static bool box_y_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b);
    static bool box_z_compare(std::shared_ptr<Hittable> const& a, std::shared_ptr<Hittable> const& b);

    std::shared_ptr<Hittable> m_hittable_left = {};
    std::shared_ptr<Hittable> m_hittable_right = {};
    std::shared_ptr<BVHNode> m_left = {};
    std::shared_ptr<BVHNode> m_right = {};
    AABB m_bbox = {};
};
