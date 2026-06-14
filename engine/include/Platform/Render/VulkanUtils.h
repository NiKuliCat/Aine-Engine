#pragma once
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Core/Log.h"
#include <set>
#include <algorithm>
namespace Aine::Render
{
	constexpr std::array<const char*, 1> s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
	constexpr std::array<const char*, 1> s_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	struct VKQueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() const
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities{};
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};



	inline VKQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physcialDevice, VkSurfaceKHR surface)
	{
		VKQueueFamilyIndices indices{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physcialDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(physcialDevice, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
			}

			VkBool32 presentSupport = VK_FALSE;

			vkGetPhysicalDeviceSurfaceSupportKHR(physcialDevice, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.PresentFamily = i;
			}

			if (indices.IsComplete())
			{
				break;
			}
		}

		return indices;

	}

	inline std::vector<const char*> GetSDLRequiredInstanceExtensions()
	{

		Uint32 count = 0;
		const char* const* SDL_Extensions = SDL_Vulkan_GetInstanceExtensions(&count);

		AINE_ASSERT(SDL_Extensions, SDL_GetError());

		std::vector<const char*> extensions;

		extensions.reserve(count);

		for (uint32_t i = 0; i < count; i++)
		{
			extensions.push_back(SDL_Extensions[i]);
		}
#ifdef AINE_DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData)
	{
		(void)type;
		(void)userData;

		if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			AINE_CORE_WARN("Vulkan validation: {0}", callbackData->pMessage);
		else
			AINE_CORE_TRACE("Vulkan validation: {0}", callbackData->pMessage);

		return VK_FALSE;
	}

	inline void VKCheck(VkResult result, const char* message)
	{
		if (result != VK_SUCCESS)
		{
			AINE_CORE_ERROR("{0}, VkResult = {1}", message, static_cast<int32_t>(result));
			throw std::runtime_error(message);
		}
	}


	inline void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = VulkanDebugCallback;
	}

	
	inline std::vector<VkPhysicalDevice> GetAllEnablePhysicalDevices(VkInstance instance)
	{
		uint32_t physicalDevicesCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);

		AINE_ASSERT(physicalDevicesCount > 0, "failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
		vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevices.data());

		return physicalDevices;
	}

	inline bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionsCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());

		std::set<std::string> requireExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			requireExtensions.erase(extension.extensionName);
		}

		return requireExtensions.empty();
	}

	inline SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapchainSupportDetails details{};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount > 0)
		{
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount > 0)
		{
			details.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
		}

		return details;
	}


	inline bool IsDeviceSuitable(VkPhysicalDevice device,VkSurfaceKHR surface)
	{
		VKQueueFamilyIndices indices = FindQueueFamilies(device, surface);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapchainAdequate = false;
		if (extensionsSupported)
		{
			SwapchainSupportDetails support = QuerySwapchainSupport(device, surface);

			swapchainAdequate = !support.Formats.empty() && !support.PresentModes.empty();
		}
		return indices.IsComplete() && extensionsSupported && swapchainAdequate;
	}

	inline VkPhysicalDevice PickPhysicalDevice(VkInstance instance,VkSurfaceKHR surface)
	{
		auto physicalDevices = GetAllEnablePhysicalDevices(instance);

		for (VkPhysicalDevice device : physicalDevices)
		{
			if (IsDeviceSuitable(device, surface))
			{
				return device;
			}
		}

		AINE_ASSERT(0, "Can not find a suitable physical device !");
		return nullptr;
	}

	inline VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		for (const auto& format : formats)
		{
			if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return formats[0];
	}

	inline VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
	{
		for (const auto& mode : presentModes)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	inline VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,SDL_Window* windowHanle)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}

		int width = 0;
		int height = 0;
		if (!SDL_GetWindowSizeInPixels(windowHanle, &width, &height))
		{
			width = 1;
			height = 1;
		}

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(
			actualExtent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width);

		actualExtent.height = std::clamp(
			actualExtent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
