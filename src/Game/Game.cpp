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
#include "Renderer/TextureCPU.h"
#include "ResourceManager.h"
#include "ScreenText.h"
#include "SoundListener.h"

Game::Game(std::shared_ptr<Window> const& window) : window(window)
{
}

static void checkered_spheres()
{
    auto const standard_shader = ResourceManager::get_instance().load_shader("./res/shaders/lit.hlsl", "./res/shaders/lit.hlsl");
    auto const standard_material = Material::create(standard_shader);

    auto const camera = Entity::create("Camera");
    camera->transform->set_local_position(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->transform->set_euler_angles(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->add_component<SoundListener>(SoundListener::create());

    auto const camera_comp = camera->add_component(Camera::create());
    camera_comp->set_can_tick(true);
    camera_comp->set_fov(glm::radians(20.0f));
    camera_comp->update();

    camera->transform->set_position({13.0f, 2.0f, 3.0f});
    camera->transform->set_euler_angles({0.0f, 260.0f, 5.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(400);
    raytracer->set_aspect_ratio(16.0f / 9.0f);
    raytracer->set_samples_per_pixel(100);
    raytracer->set_max_depth(50);

    auto const material = Material::create(standard_shader);
    material->texture = std::make_shared<CheckerTexture>(0.32f, glm::vec3(0.2f, 0.3f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));

    auto const sphere1 = Entity::create("Sphere1");
    sphere1->transform->set_position({0.0f, -10.0f, 0.0f});
    sphere1->add_component<SphereRaytraced>(SphereRaytraced::create(10.0f, material));

    auto const sphere2 = Entity::create("Sphere2");
    sphere2->transform->set_position({0.0f, 10.0f, 0.0f});
    sphere2->add_component<SphereRaytraced>(SphereRaytraced::create(10.0f, material));

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
}

static void bouncing_spheres_scene()
{
    auto const standard_shader = ResourceManager::get_instance().load_shader("./res/shaders/lit.hlsl", "./res/shaders/lit.hlsl");
    auto const standard_material = Material::create(standard_shader);

    auto const camera = Entity::create("Camera");
    camera->transform->set_local_position(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->transform->set_euler_angles(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->add_component<SoundListener>(SoundListener::create());

    auto const camera_comp = camera->add_component(Camera::create());
    camera_comp->set_can_tick(true);
    camera_comp->set_fov(glm::radians(20.0f));
    camera_comp->update();

    camera->transform->set_position({13.0f, 2.0f, 3.0f});
    camera->transform->set_euler_angles({0.0f, 270.0f, 0.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(1200);
    raytracer->set_aspect_ratio(16.0f / 9.0f);
    raytracer->set_samples_per_pixel(100);
    raytracer->set_max_depth(50);

    auto const material_ground = Material::create(standard_shader);
    material_ground->texture = std::make_shared<CheckerTexture>(0.32f, glm::vec3(0.2f, 0.3f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));

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

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
}

i32 scene_index = 2;

void Game::initialize()
{
    switch (scene_index)
    {
    case 0:
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
        break;
    }
    case 1:
    {
        bouncing_spheres_scene();
        break;
    }
    case 2:
    {
        checkered_spheres();
        break;
    }
    default:
    {
        break;
    }
    }
}
