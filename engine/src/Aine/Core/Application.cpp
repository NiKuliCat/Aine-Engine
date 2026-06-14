#include <iostream>
#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/Time.h"


#include "Window/SDLWindow.h"
namespace Aine
{
    Application* Application::s_Instance = nullptr; 

	Application::Application(const ApplicationDesc& appDesc)
        :m_Desc(appDesc), m_Active(true)
	{
        s_Instance = this;
	}

    int Application::Run()
	{
        InitLogSystem();

        InitWindow();
        InitTimeSystem();
        InitRenderer();
        InitImGuiLayer();
        InitializePendingLayers();
        MainLoop();

        Shutdown();


        return 1;
	}

    void Application::InitWindow()
    {
        WindowCreateInfo createInfo{};
        createInfo.Title = m_Desc.AppName;
        createInfo.Width = m_Desc.Width;
        createInfo.Height = m_Desc.Height;
        createInfo.VSync = true;
        m_Window = Window::Create(createInfo);

        m_Window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));
    }

    void Application::InitLogSystem()
    {
        Log::Init();
    }

    void Application::InitTimeSystem()
    {
        Time::Init();
    }

    void Application::InitRenderer()
    {
        m_Renderer = CreateRef<Render::Renderer>(m_Window->GetHandle());
    }
    void Application::InitImGuiLayer()
    {
        m_ImGuiLayer = CreateUnique<ImGuiLayer>();
        PushOverlay(m_ImGuiLayer.get());

    }
    void Application::InitializePendingLayers()
    {
        if (m_LayersInitialized)
            return;

        if (m_ImGuiLayer && !m_ImGuiLayer->IsCreated())
        {
            m_ImGuiLayer->OnCreate();
        }

        for (Layer* layer : m_LayerStack)
        {
            if (!layer || layer->IsCreated())
                continue;

            layer->OnCreate();
        }

        m_LayersInitialized = true;
    }
    void Application::MainLoop()
    {
        while (m_Active)
        {
            PollEvent();
            m_Window->OnUpdate();
            Time::OnUpdate();

            float deltaTime = Time::GetDeltaTime();
            //float totalTime = Time::GetTotalTime();
            //AINE_CORE_DEBUG("delta : {0},   total : {1}",deltaTime,totalTime);

            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate(deltaTime);
            }

        }
    }

    void Application::Shutdown()
    {
        if (m_Window)
        {
            m_Renderer->OnDestroy();
            m_Window->OnDestroy();
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDisPatcher dispatcher(event);
        dispatcher.Dispatcher<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClosed));
        dispatcher.Dispatcher<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnWindowResize));
    }
    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        if (m_LayersInitialized && layer && !layer->IsCreated())
        {
            layer->OnCreate();
        }
    }
    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        if (m_LayersInitialized && overlay && !overlay->IsCreated())
        {
            overlay->OnCreate();
        }
    }
    bool Application::OnWindowClosed(WindowCloseEvent& event)
    {
        m_Active = false;
        return true;
    }
    bool Application::OnWindowResize(WindowResizeEvent& event)
    {
       // AINE_CORE_TRACE(event.ToString());
        return false;
    }

    void Application::PollEvent()
    {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent))
        {
            if (m_ImGuiLayer)
                m_ImGuiLayer->ProcessEvent(sdlEvent);

            if (auto sdlWindow = dynamic_cast<SDLWindow*>(m_Window.get()))
                sdlWindow->DispatchSDLEvent(sdlEvent);
        }
    }


    int RunApplication(std::unique_ptr<Application> app)
    {
        if (!app)
        {
            return 1;
        }

        return app->Run();
    }

}