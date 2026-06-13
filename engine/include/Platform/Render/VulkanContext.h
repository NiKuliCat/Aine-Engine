#pragma once
#include <vulkan/vulkan.h>

#include "Render/RHIContext.h"

namespace Aine::Render
{
	class VulkanContext :public RHIContext
	{
	public:
		VulkanContext();

		~VulkanContext();

		virtual void Init(void* windowHandle) override;

		virtual void OnDestroy() override;
		virtual void OnResize(uint32_t width, uint32_t height) override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;


	private:
		void CreateInstance();

		

	private:
		void* m_WindowHandle = nullptr;
		VkInstance m_Instance = VK_NULL_HANDLE;

	};
}
