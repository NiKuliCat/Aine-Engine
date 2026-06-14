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
	}
	void VulkanContext::OnDestroy()
	{
	}
	void VulkanContext::OnResize(uint32_t width, uint32_t height)
	{
	}
	void VulkanContext::BeginFrame()
	{
	}
	void VulkanContext::EndFrame()
	{
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



}
