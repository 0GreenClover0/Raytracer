#include "Game.h"

#include <glm/gtc/random.hpp>

#include "Camera.h"
#include "Collider2D.h"
#include "CommonEntities.h"
#include "Entity.h"
#include "ExampleUIBar.h"
#include "MeshFactory.h"
#include "Model.h"
#include "Renderer/ConstantDensityMedium.h"
#include "Renderer/QuadRaytraced.h"
#include "Renderer/Raytracer.h"
#include "Renderer/RotateYHittable.h"
#include "Renderer/SphereRaytraced.h"
#include "Renderer/TextureCPU.h"
#include "Renderer/TranslateHittable.h"
#include "ResourceManager.h"
#include "ScreenText.h"
#include "SoundListener.h"

Game::Game(std::shared_ptr<Window> const& window) : window(window)
{
}

static void cornell_smoke()
{
    auto const standard_shader = ResourceManager::get_instance().load_shader("./res/shaders/lit.hlsl", "./res/shaders/lit.hlsl");
    auto const standard_material = Material::create(standard_shader);

    auto const camera = Entity::create("Camera");
    camera->add_component<SoundListener>(SoundListener::create());

    auto const camera_comp = camera->add_component(Camera::create());
    camera_comp->set_can_tick(true);
    camera_comp->set_fov(glm::radians(40.0f));
    camera_comp->update();

    camera->transform->set_position({278.0f, 278.0f, -800.0f});
    camera->transform->set_euler_angles({0.0f, 0.0f, 0.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(600);
    raytracer->set_aspect_ratio(1.0f);
    raytracer->set_samples_per_pixel(200);
    raytracer->set_max_depth(50);
    raytracer->set_background_color({0.0f, 0.0f, 0.0f});

    auto const red_material = Material::create(standard_shader);
    red_material->color = {0.65f, 0.05f, 0.05f, 1.0f};

    auto const white_material = Material::create(standard_shader);
    white_material->color = {0.73f, 0.73f, 0.73f, 1.0f};

    auto const green_material = Material::create(standard_shader);
    green_material->color = {0.12f, 0.45f, 0.15f, 1.0f};

    auto const fog_material = Material::create(standard_shader);
    fog_material->texture = std::make_shared<SolidColor>(glm::vec3(0.0f, 0.0f, 0.0f));
    fog_material->isotropic = true;

    auto const smoke_material = Material::create(standard_shader);
    smoke_material->texture = std::make_shared<SolidColor>(glm::vec3(1.0f, 1.0f, 1.0f));
    smoke_material->isotropic = true;

    auto const light_material = Material::create(standard_shader);
    light_material->color = {};
    light_material->emmisive = true;
    light_material->texture = std::make_shared<SolidColor>(glm::vec3 {7.0f, 7.0f, 7.0f});

    auto const green_quad = Entity::create("GreenQuad");
    green_quad->add_component<QuadRaytraced>(
        QuadRaytraced::create({555.0f, 0.0f, 0.0f}, {0.0f, 555.0f, 0.0f}, {0.0f, 0.0f, 555.0f}, green_material));

    auto const red_quad = Entity::create("RedQuad");
    red_quad->add_component<QuadRaytraced>(
        QuadRaytraced::create({0.0f, 0.0f, 0.0f}, {0.0f, 555.0f, 0.0f}, {0.0f, 0.0f, 555.0f}, red_material));

    auto const light_quad = Entity::create("LightQuad");
    light_quad->add_component<QuadRaytraced>(
        QuadRaytraced::create({113.0f, 554.0f, 127.0f}, {330.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 305.0f}, light_material));

    auto const white_quad1 = Entity::create("WhiteQuad1");
    white_quad1->add_component<QuadRaytraced>(
        QuadRaytraced::create({0.0f, 0.0f, 0.0f}, {555.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 555.0f}, white_material));

    auto const white_quad2 = Entity::create("WhiteQuad2");
    white_quad2->add_component<QuadRaytraced>(
        QuadRaytraced::create({555.0f, 555.0f, 555.0f}, {-555.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -555.0f}, white_material));

    auto const white_quad3 = Entity::create("WhiteQuad3");
    white_quad3->add_component<QuadRaytraced>(
        QuadRaytraced::create({0.0f, 0.0f, 555.0f}, {555.0f, 0.0f, 0.0f}, {0.0f, 555.0f, 0.0f}, white_material));

    auto sides = QuadRaytraced::box({0.0f, 0.0f, 0.0f}, {165.0f, 330.0f, 165.0f}, white_material);

    for (auto& side : sides)
    {
        side = side->entity->add_component<RotateYHittable>(std::make_shared<RotateYHittable>(side, 15.0f));
        side = side->entity->add_component<TranslateHittable>(std::make_shared<TranslateHittable>(side, glm::vec3(265.0f, 0.0f, 295.0f)));
    }

    auto const fog = Entity::create("Fog");
    fog->add_component<ConstantDensityMedium>(
        std::make_shared<ConstantDensityMedium>(std::vector(sides.begin(), sides.end()), 0.01f, fog_material));

    sides = QuadRaytraced::box({0.0f, 0.0f, 0.0f}, {165.0f, 165.0f, 165.0f}, white_material);

    for (auto& side : sides)
    {
        side = side->entity->add_component<RotateYHittable>(std::make_shared<RotateYHittable>(side, -18.0f));
        side = side->entity->add_component<TranslateHittable>(std::make_shared<TranslateHittable>(side, glm::vec3(130.0f, 0.0f, 65.0f)));
    }

    auto const smoke = Entity::create("Smoke");
    smoke->add_component<ConstantDensityMedium>(
        std::make_shared<ConstantDensityMedium>(std::vector(sides.begin(), sides.end()), 0.01f, smoke_material));

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
}

static void cornell_box()
{
    auto const standard_shader = ResourceManager::get_instance().load_shader("./res/shaders/lit.hlsl", "./res/shaders/lit.hlsl");
    auto const standard_material = Material::create(standard_shader);

    auto const camera = Entity::create("Camera");
    camera->add_component<SoundListener>(SoundListener::create());

    auto const camera_comp = camera->add_component(Camera::create());
    camera_comp->set_can_tick(true);
    camera_comp->set_fov(glm::radians(40.0f));
    camera_comp->update();

    camera->transform->set_position({278.0f, 278.0f, -800.0f});
    camera->transform->set_euler_angles({0.0f, 0.0f, 0.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(600);
    raytracer->set_aspect_ratio(1.0f);
    raytracer->set_samples_per_pixel(200);
    raytracer->set_max_depth(50);
    raytracer->set_background_color({0.0f, 0.0f, 0.0f});

    auto const red_material = Material::create(standard_shader);
    red_material->color = {0.65f, 0.05f, 0.05f, 1.0f};

    auto const white_material = Material::create(standard_shader);
    white_material->color = {0.73f, 0.73f, 0.73f, 1.0f};

    auto const green_material = Material::create(standard_shader);
    green_material->color = {0.12f, 0.45f, 0.15f, 1.0f};

    auto const light_material = Material::create(standard_shader);
    light_material->color = {};
    light_material->emmisive = true;
    light_material->texture = std::make_shared<SolidColor>(glm::vec3 {15.0f, 15.0f, 15.0f});

    auto const green_quad = Entity::create("GreenQuad");
    green_quad->add_component<QuadRaytraced>(
        QuadRaytraced::create({555.0f, 0.0f, 0.0f}, {0.0f, 555.0f, 0.0f}, {0.0f, 0.0f, 555.0f}, green_material));

    auto const red_quad = Entity::create("RedQuad");
    red_quad->add_component<QuadRaytraced>(
        QuadRaytraced::create({0.0f, 0.0f, 0.0f}, {0.0f, 555.0f, 0.0f}, {0.0f, 0.0f, 555.0f}, red_material));

    auto const light_quad = Entity::create("LightQuad");
    light_quad->add_component<QuadRaytraced>(
        QuadRaytraced::create({343.0f, 554.0f, 332.0f}, {-130.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -105.0f}, light_material));

    auto const white_quad1 = Entity::create("WhiteQuad1");
    white_quad1->add_component<QuadRaytraced>(
        QuadRaytraced::create({0.0f, 0.0f, 0.0f}, {555.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 555.0f}, white_material));

    auto const white_quad2 = Entity::create("WhiteQuad2");
    white_quad2->add_component<QuadRaytraced>(
        QuadRaytraced::create({555.0f, 555.0f, 555.0f}, {-555.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -555.0f}, white_material));

    auto const white_quad3 = Entity::create("WhiteQuad3");
    white_quad3->add_component<QuadRaytraced>(
        QuadRaytraced::create({0.0f, 0.0f, 555.0f}, {555.0f, 0.0f, 0.0f}, {0.0f, 555.0f, 0.0f}, white_material));

    auto sides = QuadRaytraced::box({0.0f, 0.0f, 0.0f}, {165.0f, 330.0f, 165.0f}, white_material);

    for (auto side : sides)
    {
        side = side->entity->add_component<RotateYHittable>(std::make_shared<RotateYHittable>(side, 15.0f));
        side = side->entity->add_component<TranslateHittable>(std::make_shared<TranslateHittable>(side, glm::vec3(265.0f, 0.0f, 295.0f)));
    }

    sides = QuadRaytraced::box({0.0f, 0.0f, 0.0f}, {165.0f, 165.0f, 165.0f}, white_material);

    for (auto side : sides)
    {
        side = side->entity->add_component<RotateYHittable>(std::make_shared<RotateYHittable>(side, -18.0f));
        side = side->entity->add_component<TranslateHittable>(std::make_shared<TranslateHittable>(side, glm::vec3(130.0f, 0.0f, 65.0f)));
    }

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
}

static void simple_light()
{
    auto const standard_shader = ResourceManager::get_instance().load_shader("./res/shaders/lit.hlsl", "./res/shaders/lit.hlsl");
    auto const standard_material = Material::create(standard_shader);

    auto const camera = Entity::create("Camera");
    camera->add_component<SoundListener>(SoundListener::create());

    auto const camera_comp = camera->add_component(Camera::create());
    camera_comp->set_can_tick(true);
    camera_comp->set_fov(glm::radians(20.0f));
    camera_comp->update();

    camera->transform->set_position({26.0f, 3.0f, 6.0f});
    camera->transform->set_euler_angles({0.0f, 260.0f, 5.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(400);
    raytracer->set_aspect_ratio(16.0f / 9.0f);
    raytracer->set_samples_per_pixel(100);
    raytracer->set_max_depth(50);
    raytracer->set_background_color({0.0f, 0.0f, 0.0f});

    auto const perlin_texture = std::make_shared<NoiseTexture>(4.0f);
    auto const material = Material::create(standard_shader);
    material->texture = perlin_texture;

    auto const sphere1 = Entity::create("Sphere1");
    sphere1->transform->set_position({0.0f, -1000.0f, 0.0f});
    sphere1->add_component<SphereRaytraced>(SphereRaytraced::create(1000.0f, material));

    auto const sphere2 = Entity::create("Sphere2");
    sphere2->transform->set_position({0.0f, 2.0f, 0.0f});
    sphere2->add_component<SphereRaytraced>(SphereRaytraced::create(2.0f, material));

    auto const light_material = Material::create(standard_shader);
    light_material->emmisive = true;
    glm::vec3 constexpr light_color = {4.0f, 4.0f, 4.0f};
    light_material->texture = std::make_shared<SolidColor>(light_color);

    auto light = Entity::create("Light");
    light->add_component<QuadRaytraced>(QuadRaytraced::create({3.0f, 1.0f, -2.0f}, {2.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f}, light_material));

    light = Entity::create("Light");
    light->transform->set_position({-10.0f, 6.0f, 5.0f});
    light->add_component<SphereRaytraced>(SphereRaytraced::create(2.0f, light_material));

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
}

static void quads()
{
    auto const standard_shader = ResourceManager::get_instance().load_shader("./res/shaders/lit.hlsl", "./res/shaders/lit.hlsl");
    auto const standard_material = Material::create(standard_shader);

    auto const camera = Entity::create("Camera");
    camera->transform->set_local_position(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->transform->set_euler_angles(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->add_component<SoundListener>(SoundListener::create());

    auto const camera_comp = camera->add_component(Camera::create());
    camera_comp->set_can_tick(true);
    camera_comp->set_fov(glm::radians(80.0f));
    camera_comp->update();

    camera->transform->set_position({0.0f, 0.0f, 9.0f});
    camera->transform->set_euler_angles({0.0f, 180.0f, 0.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(400);
    raytracer->set_aspect_ratio(1.0f);
    raytracer->set_samples_per_pixel(100);
    raytracer->set_max_depth(50);
    raytracer->set_background_color({0.7f, 0.8f, 1.0f});

    auto const quad1 = Material::create(standard_shader);
    auto const quad2 = Material::create(standard_shader);
    auto const quad3 = Material::create(standard_shader);
    auto const quad4 = Material::create(standard_shader);
    auto const quad5 = Material::create(standard_shader);
    quad1->color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
    quad2->color = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);
    quad3->color = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);
    quad4->color = glm::vec4(1.0f, 0.5f, 00.f, 1.0f);
    quad5->color = glm::vec4(0.2f, 0.8f, 0.8f, 1.0f);

    auto quad = Entity::create("Quad");
    quad->add_component<QuadRaytraced>(QuadRaytraced::create({-3.0f, -2.0f, 5.0f}, {0.0f, 0.0f, -4.0f}, {0.0f, 4.0f, 0.0f}, quad1));

    quad = Entity::create("Quad");
    quad->add_component<QuadRaytraced>(QuadRaytraced::create({-2.0f, -2.0f, 0.0f}, {4.0f, 0.0f, 0.0f}, {0.0f, 4.0f, 0.0f}, quad2));

    quad = Entity::create("Quad");
    quad->add_component<QuadRaytraced>(QuadRaytraced::create({3.0f, -2.0f, 1.0f}, {0.0f, 0.0f, 4.0f}, {0.0f, 4.0f, 0.0f}, quad3));

    quad = Entity::create("Quad");
    quad->add_component<QuadRaytraced>(QuadRaytraced::create({-2.0f, 3.0f, 1.0f}, {4.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 4.0f}, quad4));

    quad = Entity::create("Quad");
    quad->add_component<QuadRaytraced>(QuadRaytraced::create({-2.0f, -3.0f, 5.0f}, {4.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -4.0f}, quad5));

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
}

static void perlin_spheres()
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
    raytracer->set_background_color({0.7f, 0.8f, 1.0f});

    auto const perlin_texture = std::make_shared<NoiseTexture>(4.0f);

    auto const material = Material::create(standard_shader);
    material->texture = perlin_texture;

    auto const sphere1 = Entity::create("Sphere1");
    sphere1->transform->set_position({0.0f, -1000.0f, 0.0f});
    sphere1->add_component<SphereRaytraced>(SphereRaytraced::create(1000.0f, material));

    auto const sphere2 = Entity::create("Sphere2");
    sphere2->transform->set_position({0.0f, 2.0f, 0.0f});
    sphere2->add_component<SphereRaytraced>(SphereRaytraced::create(2.0f, material));

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
}

static void earth()
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

    camera->transform->set_position({0.0f, 0.0f, 12.0f});
    camera->transform->set_euler_angles({0.0f, -180.0f, 0.0f});

    auto const raytracer = Raytracer::create();

    raytracer->set_image_width(400);
    raytracer->set_aspect_ratio(16.0f / 9.0f);
    raytracer->set_samples_per_pixel(100);
    raytracer->set_max_depth(50);
    raytracer->set_background_color({0.7f, 0.8f, 1.0f});

    auto earth_image = ResourceManager::get_instance().load_image("./res/textures/images/flowers.jpg");
    auto const earth_surface = Material::create(standard_shader);
    earth_surface->texture = std::make_shared<ImageTexture>(earth_image);

    auto const sphere1 = Entity::create("Globe");
    sphere1->transform->set_position({0.0f, 0.0f, 0.0f});
    sphere1->add_component<SphereRaytraced>(SphereRaytraced::create(2.0f, earth_surface));

    raytracer->initialize(camera_comp);

    raytracer->render(camera_comp);
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
    raytracer->set_background_color({0.7f, 0.8f, 1.0f});

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
    raytracer->set_background_color({0.7f, 0.8f, 1.0f});

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

i32 scene_index = 8;

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
    case 3:
    {
        earth();
        break;
    }
    case 4:
    {
        perlin_spheres();
        break;
    }
    case 5:
    {
        quads();
        break;
    }
    case 6:
    {
        simple_light();
        break;
    }
    case 7:
    {
        cornell_box();
        break;
    }
    case 8:
    {
        cornell_smoke();
        break;
    }
    default:
    {
        break;
    }
    }
}
