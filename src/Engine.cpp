#include "Engine.h"

#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <miniaudio.h>

#include "AssetPreloader.h"
#include "Editor.h"
#include "Game/Game.h"
#include "Globals.h"
#include "Input.h"
#include "MainScene.h"
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "RendererDX11.h"
#include "SceneSerializer.h"
#include "Window.h"

#if EDITOR
#include <ImGuizmo.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#endif

i32 Engine::initialize()
{
    if (auto const result = initialize_thirdparty_before_renderer(); result != 0)
        return result;

    switch (Renderer::renderer_api)
    {
    case Renderer::RendererApi::DirectX11:
    {
        static_cast<void>(RendererDX11::create());
        break;
    }
    default:
    {
        std::unreachable();
    }
    }

    Renderer::get_instance()->set_vsync(enable_vsync);

    PhysicsEngine::get_instance()->initialize();

    if (auto const result = initialize_thirdparty_after_renderer(); result != 0)
        return result;

    asset_preloader = AssetPreloader::create();

    InternalMeshData::initialize();

    // It shouldn't be done too early, that's why it's here
    // and not eg. in Window class right after glfw window creation.
    window->maximize_glfw_window();

#if EDITOR
    m_editor = Editor::Editor::create();
#endif

    return 0;
}

void Engine::create_game()
{
    auto const main_scene = std::make_shared<Scene>();
    MainScene::set_instance(main_scene);

#if EDITOR
    m_editor->set_scene(main_scene);
#endif

    // Custom initialization code
    auto const game = std::make_shared<Game>(window);

    game->initialize();

#if EDITOR
    bool const loaded = m_editor->load_scene();

    if (!loaded)
    {
        std::cout << "Scene could not be loaded.\n";
        return;
    }
#endif

#if !EDITOR
    auto const scene_serializer = std::make_shared<SceneSerializer>(main_scene);
    scene_serializer->set_instance(scene_serializer);

    std::string const scene = "./res/scenes/MainScene.txt";
    if (!scene_serializer->deserialize(scene))
    {
        std::cout << "Scene could not be loaded.\n";
        return;
    }

    scene_serializer->set_instance(nullptr);

    Renderer::get_instance()->set_rendering_to_texture(false);

    glfwSetInputMode(Engine::window->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    GLFWvidmode const* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowMonitor(Engine::window->get_glfw_window(), glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);

    Engine::set_game_running(true);
#endif
}

void Engine::run()
{
    Renderer::get_instance()->initialize();

    double last_frame = 0.0; // Time of last frame

    // Main loop
    while (!glfwWindowShouldClose(window->get_glfw_window()) && !should_exit)
    {
        double const current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        glfwPollEvents();
        Input::input->update_keys();

#if EDITOR
        // Start the Dear ImGui frame
        switch (Renderer::renderer_api)
        {
        case Renderer::RendererApi::DirectX11:
        {
            ImGui_ImplDX11_NewFrame();
            break;
        }
        default:
        {
            std::unreachable();
        }
        }

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        m_editor->set_docking_space();
        ImGuizmo::BeginFrame();

        m_editor->handle_input();
        m_editor->draw();
#endif

        Renderer::get_instance()->begin_frame();

        if (m_is_game_running && !m_is_game_paused)
        {
            PhysicsEngine::get_instance()->update_physics();
            MainScene::get_instance()->run_frame();
        }

        Renderer::get_instance()->render();

        Renderer::get_instance()->end_frame();

#if EDITOR
        ImGui::Render();
#endif

        switch (Renderer::renderer_api)
        {
        case Renderer::RendererApi::DirectX11:
        {
#if EDITOR
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

            break;
        }
        default:
        {
            std::unreachable();
        }
        }

        Renderer::get_instance()->present();
    }
}

void Engine::clean_up()
{
    Renderer::get_instance()->uninitialize();

    switch (Renderer::renderer_api)
    {
    case Renderer::RendererApi::DirectX11:
    {
#if EDITOR
        ImGui_ImplDX11_Shutdown();
#endif
        break;
    }
    default:
    {
        std::unreachable();
    }
    }

#if EDITOR
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
#endif

    glfwDestroyWindow(window->get_glfw_window());
    glfwTerminate();
}

bool Engine::is_game_running()
{
    return m_is_game_running;
}

void Engine::set_game_running(bool const is_running)
{
    if (is_running == m_is_game_running)
        return;

    if (m_is_game_running)
    {
        uninitialize_miniaudio();

        MainScene::get_instance()->unload();

        MainScene::set_instance(nullptr);

        create_game();

        set_game_paused(false);
    }
    else
    {
        initialize_miniaudio();
    }

    m_is_game_running = is_running;
}

bool Engine::is_game_paused()
{
    return m_is_game_paused;
}

void Engine::set_game_paused(bool const is_paused)
{
    if (m_is_game_paused == is_paused)
        return;

    m_is_game_paused = is_paused;
}

std::shared_ptr<Window> Engine::create_window()
{
    // Create window with graphics context
    auto new_window = std::make_shared<Window>(Renderer::renderer_api, Renderer::screen_width, Renderer::screen_height, 4, enable_vsync,
                                               enable_mouse_capture);

    return new_window;
}

i32 Engine::initialize_thirdparty_before_renderer()
{
    if (setup_glfw() != 0)
        return 1;

    window = create_window();

    if (window == nullptr)
        return 2;

    // NOTE: Creating input callbacks needs to happen BEFORE setting up imgui (for some unknown reason)
    auto const input_system = std::make_shared<Input>(window);
    Input::set_input(input_system);

    srand(static_cast<u32>(glfwGetTime()));

    return 0;
}

i32 Engine::initialize_thirdparty_after_renderer()
{
    setup_imgui(window->get_glfw_window());

    return 0;
}

i32 Engine::setup_glfw()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    return 0;
}

void Engine::glfw_error_callback(i32 const error, char const* description)
{
    (void)fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void Engine::setup_imgui(GLFWwindow* glfw_window)
{
#if EDITOR
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // GL 4.3 + GLSL 430
    auto const glsl_version = "#version 430";

    switch (Renderer::renderer_api)
    {
    case Renderer::RendererApi::DirectX11:
    {
        std::shared_ptr<RendererDX11> const renderer_dx = std::dynamic_pointer_cast<RendererDX11>(Renderer::get_instance());
        ImGui_ImplGlfw_InitForOther(glfw_window, true);
        ImGui_ImplDX11_Init(renderer_dx->get_device(), renderer_dx->get_device_context());
        break;
    }
    default:
    {
        std::unreachable();
    }
    }

    // Setup style
    ImGui::StyleColorsDark();
#endif
}

i32 Engine::initialize_miniaudio()
{
    ma_engine_config config = ma_engine_config_init();
    config.channels = 2;
    config.sampleRate = 48000;
    config.listenerCount = 1;

    if (ma_engine_init(&config, &audio_engine) != MA_SUCCESS)
        return -1;

    ma_device_set_master_volume(audio_engine.pDevice, 0.2f);

    return 0;
}

void Engine::uninitialize_miniaudio()
{
    ma_engine_uninit(&audio_engine);
}
