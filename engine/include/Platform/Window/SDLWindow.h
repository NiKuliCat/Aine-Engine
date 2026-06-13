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

	private:
		void Init();


	private:
		WindowProps m_WindowProps;
		SDL_Window* m_WindowHandle = nullptr;

	};
}