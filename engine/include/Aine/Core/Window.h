#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include "Core/Core.h"
#include "Event/Event.h"
namespace Aine
{

	struct WindowCreateInfo
	{
		std::string Title = "Aine Engine";
		uint32_t Width = 1960;
		uint32_t Height = 1080;
		bool VSync = true;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() = default;

	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnDestroy() = 0;
		virtual void* GetHandle() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		static Ref<Window> Create(const WindowCreateInfo& info);

	};
}