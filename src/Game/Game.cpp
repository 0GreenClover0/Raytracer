#include "Game.h"

#include <glm/gtc/random.hpp>

#include "Camera.h"
#include "Collider2D.h"
#include "CommonEntities.h"
#include "Entity.h"
#include "ExampleUIBar.h"
#include "MeshFactory.h"
#include "Model.h"
#include "Raytracer.h"
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
    m_camera_comp->set_fov(glm::radians(22.0f));
    m_camera_comp->update();

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(400);
    raytracer->set_aspect_ratio(16.0f / 9.0f);
    raytracer->set_samples_per_pixel(100);
    raytracer->set_max_depth(50);

    raytracer->initialize(m_camera_comp);

    auto const sphere1 = Entity::create("Sphere1");
    sphere1->add_component<SphereRaytraced>(SphereRaytraced::create({0.0f, 0.0f, -1.0f}, 0.5f, standard_material));
    auto const sphere2 = Entity::create("Sphere2");
    sphere2->add_component<SphereRaytraced>(SphereRaytraced::create({0.0f, -100.5f, -1.0f}, 100.0f, standard_material));

    raytracer->render(m_camera_comp);
}
