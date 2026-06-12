#pragma once
#include <string>
#include <cstdint>
#include <memory>


namespace Aine
{
    struct ApplicationDesc
    {
        std::string AppName = "Aine Engine";
        int Width = 1960;
        int Height = 1080;
    };

    class Application
    {
    public:
        Application(const ApplicationDesc& appDesc);
        virtual ~Application() {};

    public:
        int Run();
        void InitWindow();

        void MainLoop();
        void Shutdown();



    private:

        bool m_Active;
        ApplicationDesc m_Desc;

    };


    int RunApplication(std::unique_ptr<Application> app);


    
    
} // namespace Aine