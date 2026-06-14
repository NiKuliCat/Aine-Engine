#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include "Core/Window.h"
#include "Core/Layer.h"
#include "Event/ApplicationEvent.h"
#include "Render/Renderer.h"
#include "ImGui/ImGuiLayer.h"
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
        void InitRenderer();
        void InitImGuiLayer();
        void InitializePendingLayers();

        void MainLoop();
        void Shutdown();

    
        void OnEvent(Event& event);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        inline static Application& GetActiveInstance() { return *s_Instance; }
        inline Ref<Window>& GetWindow() { return m_Window; }
        inline Ref<Render::Renderer> GetRenderer() { return m_Renderer; }
    private:
        bool OnWindowClosed(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

        void PollEvent();
    private:
        static Application* s_Instance;
        bool m_Active;
        bool m_LayersInitialized = false;
        ApplicationDesc m_Desc;
        Ref<Window> m_Window = nullptr;

        Ref<Render::Renderer> m_Renderer = nullptr;
        Unique<ImGuiLayer> m_ImGuiLayer = nullptr;
        LayerStack m_LayerStack;
    };


    int RunApplication(std::unique_ptr<Application> app);


    
    
} // namespace Aine