#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include <iostream>
namespace Aine
{

	Application::Application(const ApplicationDesc& appDesc)
        :m_Desc(appDesc), m_Active(true)
	{

	}

    int Application::Run()
	{
        InitLogSystem();

        InitWindow();
        InitTimeSystem();
        InitRenderer();

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
    void Application::MainLoop()
    {
        while (m_Active)
        {
            m_Window->OnUpdate();
            Time::OnUpdate();

            //float deltaTime = Time::GetDeltaTime();
            //float totalTime = Time::GetTotalTime();
            //AINE_CORE_DEBUG("delta : {0},   total : {1}",deltaTime,totalTime);
        }
    }

    void Application::Shutdown()
    {
        if (m_Window)
        {
            m_Window->OnDestroy();
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDisPatcher dispatcher(event);
        dispatcher.Dispatcher<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClosed));
        dispatcher.Dispatcher<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnWindowResize));
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


    int RunApplication(std::unique_ptr<Application> app)
    {
        if (!app)
        {
            return 1;
        }

        return app->Run();
    }

}