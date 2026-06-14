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



}
