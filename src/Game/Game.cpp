#include "Game.h"

#include <glm/gtc/random.hpp>

#include "Camera.h"
#include "Collider2D.h"
#include "CommonEntities.h"
#include "Entity.h"
#include "ExampleUIBar.h"
#include "MeshFactory.h"
#include "Model.h"
#include "Renderer/Raytracer.h"
#include "Renderer/SphereRaytraced.h"
#include "ResourceManager.h"
#include "ScreenText.h"
#include "SoundListener.h"

Game::Game(std::shared_ptr<Window> const& window) : window(window)
{
}

void Game::initialize()
{
    auto const standard_shader = ResourceManager::get_instance().load_shader("./res/shaders/lit.hlsl", "./res/shaders/lit.hlsl");
    auto const plain_shader = ResourceManager::get_instance().load_shader("./res/shaders/simple.hlsl", "./res/shaders/simple.hlsl");
    auto const light_source_shader =
        ResourceManager::get_instance().load_shader("./res/shaders/light_source.hlsl", "./res/shaders/light_source.hlsl");
    auto const halo_shader = ResourceManager::get_instance().load_shader("./res/shaders/halo.hlsl", "./res/shaders/halo.hlsl");

    auto const standard_material = Material::create(standard_shader);
    auto const plain_material = Material::create(plain_shader);
    auto const light_source_material = Material::create(light_source_shader);
    auto const halo_material = Material::create(halo_shader);

    auto const ui_shader = ResourceManager::get_instance().load_shader("./res/shaders/ui.hlsl", "./res/shaders/ui.hlsl");
    auto const ui_material = Material::create(ui_shader, 1);
    ui_material->casts_shadows = false;

    m_camera = Entity::create("Camera");
    m_camera->transform->set_local_position(glm::vec3(0.0f, 0.0f, 0.0f));
    m_camera->transform->set_euler_angles(glm::vec3(0.0f, 0.0f, 0.0f));
    m_camera->add_component<SoundListener>(SoundListener::create());

    m_camera_comp = m_camera->add_component(Camera::create());
    m_camera_comp->set_can_tick(true);
    m_camera_comp->set_fov(glm::radians(20.0f));
    m_camera_comp->update();

    m_camera->transform->set_position({13.0f, 2.0f, 3.0f});
    m_camera->transform->set_euler_angles({0.0f, 270.0f, 0.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(1200);
    raytracer->set_aspect_ratio(16.0f / 9.0f);
    raytracer->set_samples_per_pixel(100);
    raytracer->set_max_depth(50);

    auto const material_ground = Material::create(standard_shader);
    material_ground->color = {0.5f, 0.5f, 0.5f, 1.0f};

    auto const ground = Entity::create("Ground");
    ground->transform->set_position({0.0f, -1000.0f, -0.0f});
    ground->add_component<SphereRaytraced>(SphereRaytraced::create(1000.0f, material_ground));

    for (i32 a = -11; a < 11; a++)
    {
        for (i32 b = -11; b < 11; b++)
        {
            float choose_mat = AK::random_float_fast();
            glm::vec3 center = glm::vec3(a + 0.9f * AK::random_float_fast(), 0.2f, b + 0.9f * AK::random_float_fast());

            if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f)
            {
                std::shared_ptr<Material> sphere_material = Material::create(standard_shader);
                auto const sphere = Entity::create("Sphere");
                sphere->transform->set_position(center);

                if (choose_mat < 0.8f)
                {
                    glm::vec3 rand1 = glm::linearRand(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
                    glm::vec3 rand2 = glm::linearRand(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
                    sphere_material->color = glm::vec4(rand1 * rand2, 1.0f);

                    sphere->add_component<SphereRaytraced>(SphereRaytraced::create(0.2f, sphere_material));
                }
                else if (choose_mat < 0.95f)
                {
                    glm::vec3 rand1 = glm::linearRand(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
                    sphere_material->color = glm::vec4(rand1, 1.0f);
                    sphere_material->fuzz = AK::random_float_fast(0.0f, 0.5f);
                    sphere_material->metal = true;

                    sphere->add_component<SphereRaytraced>(SphereRaytraced::create(0.2f, sphere_material));
                }
                else
                {
                    sphere_material->dielectric = true;
                    sphere_material->refraction_index = 1.5f;

                    sphere->add_component<SphereRaytraced>(SphereRaytraced::create(0.2f, sphere_material));
                }
            }
        }
    }

    auto const material1 = Material::create(standard_shader);
    material1->color = {0.4f, 0.2f, 0.1f, 1.0f};

    auto const material2 = Material::create(standard_shader);
    material2->color = {0.8f, 0.8f, 0.8f, 1.0f};
    material2->dielectric = true;
    material2->refraction_index = 1.5f;

    auto const material3 = Material::create(standard_shader);
    material3->color = {0.7f, 0.6f, 0.5f, 1.0f};
    material3->metal = true;
    material3->fuzz = 0.0f;

    auto const sphere2 = Entity::create("Sphere2");
    sphere2->transform->set_position({4.0f, 1.0f, 0.0f});
    sphere2->add_component<SphereRaytraced>(SphereRaytraced::create(1.0f, material3));

    auto const sphere1 = Entity::create("Sphere1");
    sphere1->transform->set_position({-4.0f, 1.0f, 0.0f});
    sphere1->add_component<SphereRaytraced>(SphereRaytraced::create(1.0f, material1));

    auto const sphere3 = Entity::create("Sphere3");
    sphere3->transform->set_position({0.0f, 1.0f, 0.0f});
    sphere3->add_component<SphereRaytraced>(SphereRaytraced::create(1.0f, material2));

    //auto const material_center = Material::create(standard_shader);
    //material_center->color = {0.1f, 0.2f, 0.5f, 1.0f};

    //auto const material_left = Material::create(standard_shader);
    //material_left->color = {0.8f, 0.8f, 0.8f, 1.0f};
    //material_left->dielectric = true;
    //material_left->refraction_index = 1.5f;

    //auto const material_right = Material::create(standard_shader);
    //material_right->color = {0.8f, 0.6f, 0.2f, 1.0f};
    //material_right->metal = true;
    //material_right->fuzz = 0.8f;

    //auto const material_bubble = Material::create(standard_shader);
    //material_bubble->color = {0.8f, 0.8f, 0.8f, 1.0f};
    //material_bubble->dielectric = true;
    //material_bubble->refraction_index = 1.0f / 1.5f;

    //auto const sphere2 = Entity::create("Sphere2");
    //sphere2->add_component<SphereRaytraced>(SphereRaytraced::create({0.0f, -100.5f, -1.0f}, 100.0f, material_ground));

    //auto const sphere1 = Entity::create("Sphere1");
    //sphere1->add_component<SphereRaytraced>(SphereRaytraced::create({0.0f, 0.0f, -1.2f}, 0.5f, material_center));

    //auto const sphere3 = Entity::create("Sphere3");
    //sphere1->add_component<SphereRaytraced>(SphereRaytraced::create({-1.0f, 0.0f, -1.0f}, 0.5f, material_left));

    //auto const sphere5 = Entity::create("Sphere5");
    //sphere1->add_component<SphereRaytraced>(SphereRaytraced::create({-1.0f, 0.0f, -1.0f}, 0.4f, material_bubble));

    //auto const sphere4 = Entity::create("Sphere4");
    //sphere1->add_component<SphereRaytraced>(SphereRaytraced::create({1.0f, 0.0f, -1.0f}, 0.5f, material_right));

    //float const r = glm::cos(glm::pi<float>() / 4.0f);

    //auto const material_l = Material::create(standard_shader);
    //material_l->color = {0.0f, 0.0f, 1.0f, 1.0f};

    //auto const material_r = Material::create(standard_shader);
    //material_r->color = {1.0f, 0.0f, 0.0f, 1.0f};

    //auto const sphere1 = Entity::create("Sphere1");
    //sphere1->add_component<SphereRaytraced>(SphereRaytraced::create({-r, 0.0f, -1.0f}, r, material_l));

    //auto const sphere2 = Entity::create("Sphere2");
    //sphere2->add_component<SphereRaytraced>(SphereRaytraced::create({r, 0.0f, -1.0f}, r, material_r));

    raytracer->initialize(m_camera_comp);

    raytracer->render(m_camera_comp);
}
