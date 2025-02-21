#include "Transform.h"

#include "AK/AK.h"
#include "Entity.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>

Transform::Transform(std::shared_ptr<Entity> const& entity) : entity(entity)
{
}

void Transform::set_position(glm::vec3 const& position)
{
    if (parent.expired())
    {
        m_local_position = position;
    }
    else
    {
        glm::vec3 const parent_global_position = parent.lock()->get_position();
        glm::vec3 new_local_position = position - parent_global_position;
        new_local_position = glm::inverse(parent.lock()->get_rotation()) * new_local_position;

        auto const is_position_modified = glm::epsilonNotEqual(new_local_position, m_local_position, 0.0001f);
        if (!is_position_modified.x && !is_position_modified.y && !is_position_modified.z)
        {
            return;
        }

        m_local_position = new_local_position;
    }

    set_dirty();
}

glm::vec3 Transform::get_position()
{
    recompute_model_matrix_if_needed();

    return m_position;
}

void Transform::set_rotation(glm::vec3 const& euler_angles)
{
    // this was null when adding individual particle component AGAIN?
    if (parent.expired())
    {
        m_local_rotation = glm::quat(glm::radians(euler_angles));
        m_euler_angles = euler_angles;
    }
    else
    {
        // Calculate the global rotation quaternion from the given euler angles
        glm::quat const global_rotation = glm::quat(glm::radians(euler_angles));

        // Get the parent's global rotation
        glm::quat const parent_global_rotation = parent.lock()->get_rotation();

        // Calculate the new local rotation by inverse of parent's rotation
        m_local_rotation = glm::inverse(parent_global_rotation) * global_rotation;

        // Convert the local rotation quaternion back to Euler angles for storage
        m_euler_angles = glm::degrees(glm::eulerAngles(m_local_rotation));
    }

    set_dirty();
}

glm::quat Transform::get_rotation()
{
    recompute_model_matrix_if_needed();

    return m_rotation;
}

void Transform::set_scale(glm::vec3 const& scale)
{
    if (parent.expired()) // If there's no parent, global scale is the same as local scale
    {
        m_local_scale = scale;
    }
    else
    {
        glm::vec3 const parent_global_scale = parent.lock()->get_scale();
        glm::vec3 const new_local_scale = scale / parent_global_scale;

        auto const is_scale_modified = glm::epsilonNotEqual(new_local_scale, m_local_scale, 0.0001f);
        if (!is_scale_modified.x && !is_scale_modified.y && !is_scale_modified.z)
        {
            return;
        }

        m_local_scale = new_local_scale;
    }

    set_dirty();
}

glm::vec3 Transform::get_scale()
{
    recompute_model_matrix_if_needed();

    return m_scale;
}

void Transform::set_local_position(glm::vec3 const& position)
{
    auto const is_position_modified = glm::epsilonNotEqual(position, m_local_position, 0.0001f);
    if (!is_position_modified.x && !is_position_modified.y && !is_position_modified.z)
    {
        return;
    }

    m_local_position = position;

    set_dirty();
}

glm::vec3 Transform::get_local_position() const
{
    return m_local_position;
}

void Transform::set_local_scale(glm::vec3 const& scale)
{
    auto const is_scale_modified = glm::epsilonNotEqual(scale, m_local_scale, 0.0001f);
    if (!is_scale_modified.x && !is_scale_modified.y && !is_scale_modified.z)
    {
        return;
    }

    m_local_scale = scale;

    set_dirty();
}

glm::vec3 Transform::get_local_scale() const
{
    return m_local_scale;
}

void Transform::set_euler_angles(glm::vec3 const& euler_angles)
{
    auto const is_rotation_modified = glm::epsilonNotEqual(euler_angles, m_euler_angles, 0.0001f);
    if (!is_rotation_modified.x && !is_rotation_modified.y && !is_rotation_modified.z)
    {
        return;
    }

    m_euler_angles = euler_angles;
    m_local_rotation = glm::quat(glm::radians(euler_angles));

    set_dirty();
}

glm::vec3 Transform::get_euler_angles() const
{
    return m_euler_angles;
}

glm::vec3 Transform::get_euler_angles_restricted() const
{
    return {glm::mod(glm::mod(m_euler_angles.x, 360.0f) + 360.0f, 360.0f), glm::mod(glm::mod(m_euler_angles.y, 360.0f) + 360.0f, 360.0f),
            glm::mod(glm::mod(m_euler_angles.z, 360.0f) + 360.0f, 360.0f)};
}

void Transform::orient_towards(glm::vec3 const& target, glm::vec3 const& up)
{
    glm::mat4 transformation = glm::lookAt(get_position(), target, glm::vec3(0.0f, 1.0f, 0.0f));
    transformation = glm::inverse(transformation);

    m_local_rotation = glm::quat_cast(transformation);
    m_euler_angles = glm::degrees(glm::eulerAngles(m_local_rotation));

    set_dirty();
}

glm::vec3 Transform::get_forward()
{
    recompute_forward_right_up_if_needed();

    return m_forward;
}

glm::vec3 Transform::get_right()
{
    recompute_forward_right_up_if_needed();

    return m_right;
}

glm::vec3 Transform::get_up()
{
    recompute_forward_right_up_if_needed();

    return m_up;
}

glm::mat4 const& Transform::get_model_matrix()
{
    recompute_model_matrix_if_needed();

    return m_model_matrix;
}

// Version for no parent
void Transform::compute_model_matrix()
{
    assert(parent.expired());

    assert(m_local_dirty || m_parent_dirty);

    m_model_matrix = get_local_model_matrix();

    m_parent_dirty = false;
}

void Transform::compute_model_matrix(glm::mat4 const& parent_global_model_matrix)
{
    assert(m_local_dirty || m_parent_dirty);

    m_model_matrix = parent_global_model_matrix * get_local_model_matrix();

    m_parent_dirty = false;
}

void Transform::compute_local_model_matrix()
{
    glm::mat4 const rotation_matrix = glm::mat4_cast(m_local_rotation);
    m_local_model_matrix = glm::translate(glm::mat4(1.0f), m_local_position) * rotation_matrix * glm::scale(glm::mat4(1.0f), m_local_scale);
    m_local_dirty = false;
}

void Transform::set_model_matrix(glm::mat4 const& matrix)
{
    m_model_matrix = matrix;

    if (parent.expired())
    {
        glm::decompose(m_model_matrix, m_local_scale, m_local_rotation, m_local_position, m_skew, m_perpective);
        m_euler_angles = glm::degrees(glm::eulerAngles(m_local_rotation));
    }
    else
    {
        glm::decompose(glm::inverse(parent.lock()->get_model_matrix()) * m_model_matrix, m_local_scale, m_local_rotation, m_local_position,
                       m_skew, m_perpective);
        m_euler_angles = glm::degrees(glm::eulerAngles(m_local_rotation));
    }

    set_dirty();
}

glm::mat4 Transform::get_local_model_matrix()
{
    if (!m_local_dirty)
        return m_local_model_matrix;

    compute_local_model_matrix();
    return m_local_model_matrix;
}

void Transform::recompute_model_matrix_if_needed()
{
    if (m_local_dirty || m_parent_dirty)
    {
        if (parent.expired())
            compute_model_matrix();
        else
            compute_model_matrix(parent.lock()->get_model_matrix());

        glm::decompose(m_model_matrix, m_scale, m_rotation, m_position, m_skew, m_perpective);
    }
}

void Transform::recompute_forward_right_up_if_needed()
{
    if (glm::epsilonEqual(m_euler_angles_when_caching, get_euler_angles(), 0.0001f) == glm::bvec3(true, true, true))
        return;

    auto const euler_angles = get_euler_angles();
    m_euler_angles_when_caching = euler_angles;

    auto direction_forward = glm::vec3(0.0f, 0.0f, -1.0f);
    direction_forward = glm::rotateX(direction_forward, glm::radians(euler_angles.x));
    direction_forward = glm::rotateY(direction_forward, glm::radians(euler_angles.y));
    direction_forward = glm::rotateZ(direction_forward, glm::radians(euler_angles.z));
    m_forward = glm::normalize(direction_forward);
    m_right = glm::normalize(glm::cross(m_forward, m_world_up));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

void Transform::add_child(std::shared_ptr<Transform> const& transform)
{
    children.emplace_back(transform);
    transform->parent = shared_from_this();
}

void Transform::remove_child(std::shared_ptr<Transform> const& transform)
{
    assert(transform->parent.lock() == shared_from_this());

    auto const it = std::ranges::find(children, transform);

    if (it == children.end())
        return;

    children.erase(it);

    transform->parent.reset();
}

void Transform::set_dirty()
{
    if (!m_local_dirty)
    {
        for (auto const& child : children)
        {
            child->set_parent_dirty();
        }
    }

    m_local_dirty = true;
    needs_bounding_box_adjusting = true;
}

void Transform::set_parent_dirty()
{
    if (!m_parent_dirty)
    {
        for (auto const& child : children)
        {
            child->set_parent_dirty();
        }
    }

    m_parent_dirty = true;
    needs_bounding_box_adjusting = true;
}

void Transform::set_parent(std::shared_ptr<Transform> const& new_parent)
{
    if (new_parent == nullptr)
    {
        if (parent.expired())
            return;

        parent.lock()->remove_child(shared_from_this());
        m_local_dirty = true;
        needs_bounding_box_adjusting = true;
        return;
    }

    if (!parent.expired())
    {
        parent.lock()->remove_child(shared_from_this());
    }

    new_parent->add_child(shared_from_this());
    m_local_dirty = true;
    needs_bounding_box_adjusting = true;
}
