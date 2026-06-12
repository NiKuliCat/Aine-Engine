#include "Aine/Core/Application.h"

namespace Aine
{

	Application::Application(const ApplicationDesc& appDesc)
        :m_Desc(appDesc)
	{
	}

    int Application::Run()
	{
        return 1;
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