#pragma once
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include "Core/Layer.h"
namespace Aine
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		virtual ~ImGuiLayer() = default;

		virtual void OnCreate() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnRender(float dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnDestroy() override;

		virtual void OnEvent(Event& event) override;

		void ProcessEvent(const SDL_Event& sdl_event);
	public:
		void Begin();
		void End();
	private:

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		
	};
}