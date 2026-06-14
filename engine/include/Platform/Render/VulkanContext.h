#pragma once
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <vector>
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
		void CreateSurface();
		void CreateLogicalDevice();
		void CreateSwapchain();
		void CreateSwapchainImageViews();
		void CreateCommandPool();
		void CreateCommandBuffers();

	private:
		SDL_Window* m_WindowHandle = nullptr;
		VkInstance m_Instance = VK_NULL_HANDLE;

		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		std::vector<VkImageLayout> m_SwapchainImageLayouts;
		VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D m_SwapchainExtent{};

		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue  = VK_NULL_HANDLE;
		

		uint32_t m_GraphicsQueueFamilyIndex = 0;
		uint32_t m_PresentQueueFamilyIndex = 0;


		std::vector<VkFence> m_ImagesInFlight;


		static constexpr uint32_t FramesInFlight = 2;
	};
}
