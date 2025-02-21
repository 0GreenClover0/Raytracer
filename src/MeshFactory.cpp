#include "MeshFactory.h"

#include "MeshDX11.h"
#include "Renderer.h"

std::shared_ptr<Mesh> MeshFactory::create(std::vector<Vertex> const& vertices, std::vector<u32> const& indices,
                                          std::vector<std::shared_ptr<Texture>> const& textures, DrawType const draw_type,
                                          std::shared_ptr<Material> const& material, DrawFunctionType const draw_function)
{
    switch (Renderer::renderer_api)
    {
    case Renderer::RendererApi::DirectX11:
    {
        auto mesh = std::make_shared<MeshDX11>(AK::Badge<MeshFactory> {}, vertices, indices, textures, draw_type, material, draw_function);
        return mesh;
    }

    default:
    {
        std::unreachable();
    }
    }
}
