#include "SkyboxFactory.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "SkyboxDX11.h"

std::shared_ptr<Skybox> SkyboxFactory::create(std::shared_ptr<Material> const& material, std::string const& path)
{
    if (Renderer::renderer_api == Renderer::RendererApi::DirectX11)
    {
        return std::make_shared<SkyboxDX11>(AK::Badge<SkyboxFactory> {}, material, path);
    }

    std::unreachable();
}

std::shared_ptr<Skybox> SkyboxFactory::create()
{
    if (Renderer::renderer_api == Renderer::RendererApi::DirectX11)
    {
        auto const skybox_shader = ResourceManager::get_instance().load_shader("./res/shaders/skybox.hlsl", "./res/shaders/skybox.hlsl");
        auto const skybox_material = Material::create(skybox_shader);
        auto skybox = std::make_shared<SkyboxDX11>(AK::Badge<SkyboxFactory> {}, skybox_material, "./res/textures/skybox/skybox.dds");
        return skybox;
    }

    std::unreachable();
}
