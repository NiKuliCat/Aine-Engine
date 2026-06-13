#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include "Core/Window.h"
#include "Event/ApplicationEvent.h"
#include <vulkan/vulkan.h>
namespace Aine
{
    struct ApplicationDesc
    {
        std::string AppName = "Aine Engine";
        uint32_t Width = 1960;
        uint32_t Height = 1080;
    };

    class Application
    {
    public:
        Application(const ApplicationDesc& appDesc);
        virtual ~Application() {};

    public:
        int Run();
        void InitWindow();
        void InitLogSystem();
        void InitTimeSystem();

        void MainLoop();
        void Shutdown();


        void OnEvent(Event& event);


    private:
        bool OnWindowClosed(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

    private:

        bool m_Active;
        ApplicationDesc m_Desc;
        Ref<Window> m_Window = nullptr;
    };


    int RunApplication(std::unique_ptr<Application> app);


    
    
} // namespace Aine