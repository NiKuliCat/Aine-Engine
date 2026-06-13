#pragma once

#include <string>
#include <cstdint>
#include "Core/Core.h"
namespace Aine
{

	struct WindowCreateInfo
	{
		std::string Title = "Aine Engine";
		uint32_t Width = 1960;
		uint32_t Height = 1080;
		bool VSync = true;
	};

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool VSync;
	};



	class Window
	{
	public:
		virtual ~Window() = default;

	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnDestroy() = 0;
		virtual void* GetHandle() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Ref<Window> Create(const WindowCreateInfo& info);

	};
}