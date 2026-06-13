#include "Render/VulkanContext.h"
#include "Core/Log.h"

#include <vector>
#include <array>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Aine::Render
{

	namespace
	{
		constexpr std::array<const char*, 1> s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

		std::vector<const char*> GetSDLRequiredInstanceExtensions()
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

		void VKCheck(VkResult result, const char* message)
		{
			if (result != VK_SUCCESS)
			{
				AINE_CORE_ERROR("{0}, VkResult = {1}", message, static_cast<int32_t>(result));
				throw std::runtime_error(message);
			}
		}


		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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
	}


	VulkanContext::VulkanContext()
	{

	}


	VulkanContext::~VulkanContext()
	{
	}

	void VulkanContext::Init(void* windowHandle)
	{
		m_WindowHandle = windowHandle;

		CreateInstance();
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

}
