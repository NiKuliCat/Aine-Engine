#include "Render/VulkanContext.h"
#include "Render/VulkanUtils.h"
#include <SDL3/SDL_vulkan.h>
namespace Aine::Render
{

	VulkanContext::VulkanContext()
	{

	}


	VulkanContext::~VulkanContext()
	{
	}

	void VulkanContext::Init(void* windowHandle)
	{
		m_WindowHandle = static_cast<SDL_Window*>(windowHandle);

		CreateInstance();
		CreateSurface();

		m_PhysicalDevice = PickPhysicalDevice(m_Instance, m_Surface);
		CreateLogicalDevice();
		CreateSwapchain();
		CreateSwapchainImageViews();

		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
	}
	void VulkanContext::OnDestroy()
	{
		if (m_Instance == VK_NULL_HANDLE)
		{
			return;
		}


		WaitIdle();


		for (size_t i = 0; i < m_ImageAvailableSemaphores.size(); ++i)
		{
			vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_LogicalDevice, m_InFlightFences[i], nullptr);
		}

		for (VkSemaphore semaphore : m_RenderFinishedSemaphores)
			vkDestroySemaphore(m_LogicalDevice, semaphore, nullptr);

		m_ImageAvailableSemaphores.clear();
		m_RenderFinishedSemaphores.clear();
		m_InFlightFences.clear();
		m_ImagesInFlight.clear();

		if (m_CommandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
			m_CommandPool = VK_NULL_HANDLE;
		}

		CleanupSwapchain();

		if (m_LogicalDevice != VK_NULL_HANDLE)
		{
			vkDestroyDevice(m_LogicalDevice, nullptr);
			m_LogicalDevice = VK_NULL_HANDLE;
		}

		if (m_Surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
			m_Surface = VK_NULL_HANDLE;
		}

		vkDestroyInstance(m_Instance, nullptr);
		m_Instance = VK_NULL_HANDLE;

	}
	void VulkanContext::OnResize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
		{
			return;
		}

		m_NeedRebuildFramebuffer = true;
	}
	void VulkanContext::BeginFrame()
	{
		AINE_ASSERT(!m_FrameStarted, "BeginFrame called while frame is already started");

		vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImageIndex);

		// 窗口尺寸变化时，Acquire 也可能返回 SUBOPTIMAL；这类情况和 OUT_OF_DATE 一样走 swapchain 重建。
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapchain();
			return;
		}

		VKCheck(result, "Faild to acquire swapchain image");
		if (m_ImagesInFlight[m_CurrentImageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(m_LogicalDevice, 1, &m_ImagesInFlight[m_CurrentImageIndex], VK_TRUE, UINT64_MAX);
		}

		m_ImagesInFlight[m_CurrentImageIndex] = m_InFlightFences[m_CurrentFrame];

		vkResetFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame]);

		VkCommandBuffer cmd = m_CommandBuffers[m_CurrentFrame];

		vkResetCommandBuffer(cmd, 0);
		BeginCommandBuffer(cmd);

		m_FrameStarted = true;

	}
	void VulkanContext::EndFrame()
	{
		AINE_ASSERT(m_FrameStarted, "EndFrame called without BeginFrame");

		VkCommandBuffer cmd = m_CommandBuffers[m_CurrentFrame];
		EndCommandBuffer(cmd);

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSemaphore renderFinishSemaphores = m_RenderFinishedSemaphores[m_CurrentImageIndex];
		VkSemaphore signalSemaphores[] = { renderFinishSemaphores };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VKCheck(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]), "Failed to submit queue !");


		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &m_CurrentImageIndex;

		VkResult result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

		m_FrameStarted = false;

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_NeedRebuildFramebuffer)
		{
			RecreateSwapchain();
			m_NeedRebuildFramebuffer = false;
		}
		else
		{
			VKCheck(result, "Failed to present Vulkan swapchain image");
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % FramesInFlight;
	}
	void VulkanContext::CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Aine Editor";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = "Aine Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_4;

		auto extensions = GetSDLRequiredInstanceExtensions();

		VkDebugUtilsMessengerCreateInfoEXT  debugCreateInfo{};

		VkInstanceCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = s_ValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;


		VKCheck(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Failed to create vulkan instance");
	}

	void VulkanContext::CreateSurface()
	{
		AINE_ASSERT(SDL_Vulkan_CreateSurface(m_WindowHandle, m_Instance, nullptr, &m_Surface), SDL_GetError());
	}

	void VulkanContext::CreateLogicalDevice()
	{

		//优先获取一份可用的图形队列和呈现队列
		VKQueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice,m_Surface);

		m_GraphicsQueueFamilyIndex = indices.GraphicsFamily.value();
		m_PresentQueueFamilyIndex = indices.PresentFamily.value();

		//图形队列和呈现队列可能是同一族的，这里为了去重，避免重复创建同一个队列族
		std::set<uint32_t> uniqueQueueFamilies = {
			indices.GraphicsFamily.value(),
			indices.PresentFamily.value()
		};

		float queuePriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures2 deviceFeatures2{};
		deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

		VkPhysicalDeviceVulkan11Features vulkan11Features{};
		vulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;

		VkPhysicalDeviceVulkan13Features vulkan13Features{};
		vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		deviceFeatures2.pNext = &vulkan11Features;
		vulkan11Features.pNext = &vulkan13Features;

		vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &deviceFeatures2);

		AINE_ASSERT(vulkan11Features.shaderDrawParameters == VK_TRUE, "vulkan physical device does not support shaderDrawParameters !");
		AINE_ASSERT(vulkan13Features.dynamicRendering == VK_TRUE, "vulkan physical device does not support dynamicRendering !");

		vulkan11Features.shaderDrawParameters = VK_TRUE;
		vulkan13Features.dynamicRendering = VK_TRUE;


		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &vulkan11Features;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = nullptr;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

		createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = s_ValidationLayers.data();


		VKCheck(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice), "Failed to create Vulkan logical device !");

		vkGetDeviceQueue(m_LogicalDevice, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, indices.PresentFamily.value(), 0, &m_PresentQueue);
	}

	void VulkanContext::CreateSwapchain()
	{
		SwapchainSupportDetails support = QuerySwapchainSupport(m_PhysicalDevice, m_Surface);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(support.Formats);

		VkPresentModeKHR presentMode = ChooseSwapPresentMode(support.PresentModes);

		VkExtent2D extent = ChooseSwapExtent(support.Capabilities,m_WindowHandle);

		uint32_t imageCount = support.Capabilities.minImageCount + 1;
		if (support.Capabilities.maxImageCount > 0 && imageCount > support.Capabilities.maxImageCount)
		{
			imageCount = support.Capabilities.maxImageCount;
		}

		uint32_t queueFamilyIndices[] = { m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex };

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (m_GraphicsQueueFamilyIndex != m_PresentQueueFamilyIndex)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}

		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = support.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VKCheck(vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_Swapchain),"Faild to create vulkan swapchain !");

		vkGetSwapchainImagesKHR(m_LogicalDevice, m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);

		vkGetSwapchainImagesKHR(m_LogicalDevice, m_Swapchain, &imageCount, m_SwapchainImages.data());
		m_SwapchainImageLayouts.assign(imageCount, VK_IMAGE_LAYOUT_UNDEFINED);
		m_ImagesInFlight.assign(imageCount, VK_NULL_HANDLE);

		m_SwapchainImageFormat = surfaceFormat.format;
		m_SwapchainExtent = extent;

		m_NeedRebuildFramebuffer = false;
	}

	void VulkanContext::CreateSwapchainImageViews()
	{
		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapchainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapchainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VKCheck(vkCreateImageView(m_LogicalDevice, &createInfo, nullptr, &m_SwapchainImageViews[i]),"Failed to create Vulkan swapchain image view");
		}

	}

	void VulkanContext::CreateCommandPool()
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;

		VKCheck(vkCreateCommandPool(m_LogicalDevice, &createInfo, nullptr, &m_CommandPool), "Faild to create command pool !");
	}

	void VulkanContext::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(FramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		

		VKCheck(vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, m_CommandBuffers.data()), "Faild to create command buffers !");
	}

	void VulkanContext::CreateSyncObjects()
	{
		m_ImageAvailableSemaphores.resize(FramesInFlight);
		m_RenderFinishedSemaphores.resize(m_SwapchainImages.size());
		m_InFlightFences.resize(FramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < FramesInFlight; ++i)
		{
			VKCheck(vkCreateSemaphore(m_LogicalDevice , &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]), "Failed to create image available semaphore");
			VKCheck(vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFences[i]), "Failed to create in-flight fence");
		}

		for (size_t i = 0; i < m_RenderFinishedSemaphores.size(); ++i)
			VKCheck(vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]), "Failed to create render finished semaphore");
	}

	void VulkanContext::CleanupSwapchain()
	{
		for (VkImageView imageView : m_SwapchainImageViews)
			vkDestroyImageView(m_LogicalDevice, imageView, nullptr);

		m_SwapchainImageViews.clear();
		m_SwapchainImages.clear();
		m_SwapchainImageLayouts.clear();
		m_ImagesInFlight.clear();

		if (m_Swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(m_LogicalDevice, m_Swapchain, nullptr);
			m_Swapchain = VK_NULL_HANDLE;
		}
	}

	void VulkanContext::RecreateSwapchain()
	{
		int width = 0;
		int height = 0;

		SDL_GetWindowSizeInPixels(m_WindowHandle, &width, &height);

		vkDeviceWaitIdle(m_LogicalDevice);

		CleanupSwapchain();

		CreateSwapchain();
		CreateSwapchainImageViews();
	}

	void VulkanContext::WaitIdle()
	{
		if (m_LogicalDevice != VK_NULL_HANDLE)
			vkDeviceWaitIdle(m_LogicalDevice);
	}

	void VulkanContext::BeginCommandBuffer(VkCommandBuffer cmd)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;

		VKCheck(vkBeginCommandBuffer(cmd, &beginInfo), "Failed to begin vulkan command buffer !");
	}

	void VulkanContext::EndCommandBuffer(VkCommandBuffer cmd)
	{
		VKCheck(vkEndCommandBuffer(cmd), "Failed to end vulkan command buffer !");
	}



}
