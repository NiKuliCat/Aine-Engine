#include "Aine/Core/Application.h"
#include "Aine/Core/Log.h"
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
      


        MainLoop();

        Shutdown();


        return 1;
	}

    void Application::InitWindow()
    {
    }

    void Application::InitLogSystem()
    {
        Log::Init();
        AINE_CORE_INFO("log system init");
    }

    void Application::MainLoop()
    {
        while (m_Active)
        {
        }
    }

    void Application::Shutdown()
    {
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