#include "Window/SDLWindow.h"
#include "Core/Log.h"
namespace Aine
{
	static bool s_SDLInitialized = false;
	SDLWindow::SDLWindow(const WindowCreateInfo& info)
		:m_WindowProps({
		.Title  = info.Title,
		.Width  = info.Width,
		.Height = info.Height,
		.VSync  = info.VSync})
	{
		Init();
	}


	void SDLWindow::Init()
	{
		if (!s_SDLInitialized)
		{
			if (!SDL_Init(SDL_INIT_VIDEO))
			{
				AINE_CORE_ERROR(SDL_GetError());
				return;
			}

			s_SDLInitialized = true;
		}

		SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

		m_WindowHandle = SDL_CreateWindow(m_WindowProps.Title.c_str(), m_WindowProps.Width, m_WindowProps.Height, flags);

		if (!m_WindowHandle)
		{
			AINE_CORE_ERROR(SDL_GetError());
		}
	}


	SDLWindow::~SDLWindow()
	{

	}

	void SDLWindow::OnCreate()
	{

	}
	void SDLWindow::OnUpdate()
	{

	}
	void SDLWindow::OnDestroy()
	{
		if (m_WindowHandle)
		{
			SDL_DestroyWindow(m_WindowHandle);
			m_WindowHandle = nullptr;
		}
		SDL_Quit();
	}




	Ref<Window> Window::Create(const WindowCreateInfo& info)
	{
		return CreateRef<SDLWindow>(info);
	}


}
