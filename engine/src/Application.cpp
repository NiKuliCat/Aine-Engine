#include "Aine/Core/Application.h"
#include <iostream>
namespace Aine
{

	Application::Application(const ApplicationDesc& appDesc)
        :m_Desc(appDesc), m_Active(true)
	{

	}

    int Application::Run()
	{
        InitWindow();
      


        MainLoop();

        Shutdown();


        return 1;
	}

    void Application::InitWindow()
    {
    }

    void Application::MainLoop()
    {
        while (m_Active)
        {
            std::cout << "main loop" << std::endl;
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