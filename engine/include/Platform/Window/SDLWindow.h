#pragma once
#include "Core/Window.h"
#include <SDL3/SDL.h>
namespace Aine
{

	class SDLWindow : public Window
	{
	public:
		SDLWindow(const WindowCreateInfo& info);

		virtual ~SDLWindow();

		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void* GetHandle() const override { return m_WindowHandle; }
		virtual uint32_t GetWidth() const override { return m_WindowProps.Width; };
		virtual uint32_t GetHeight() const override { return m_WindowProps.Height; };

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_WindowProps.EventCallback = callback; }
		void DispatchSDLEvent(const SDL_Event& event);
	private:
		void Init();

	private:

		struct WindowProps
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;
			bool VSync;
			EventCallbackFn EventCallback;
		};

		WindowProps m_WindowProps;
		SDL_Window* m_WindowHandle = nullptr;

	};
}