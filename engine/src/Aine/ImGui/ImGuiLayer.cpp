#include "ImGui/ImGuiLayer.h"
#include "Render/VulkanUtils.h"
#include "Render/VulkanContext.h"
#include "Core/Application.h"
#include "Core/Log.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include <array>

namespace Aine
{
	ImGuiLayer::ImGuiLayer()
		:Layer("Main ImGui Layer")
	{
	}
	void ImGuiLayer::OnCreate()
	{
		Layer::OnCreate();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

		// Setup Platform/Renderer backends

		auto& app_instance = Application::GetActiveInstance();
		auto vulkan_context = std::dynamic_pointer_cast<Render::VulkanContext>(app_instance.GetRenderer()->GetContext());

		if (m_DescriptorPool == VK_NULL_HANDLE)
		{
			std::array<VkDescriptorPoolSize, 1> poolSizes{};
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[0].descriptorCount = 1024;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			poolInfo.maxSets = 1024;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();


			Render::VKCheck(vkCreateDescriptorPool(vulkan_context->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool),"Failed to create ImGui Vulkan descriptor pool");
		}



		ImGui_ImplSDL3_InitForVulkan(static_cast<SDL_Window*>(app_instance.GetWindow()->GetHandle()));
		ImGui_ImplVulkan_InitInfo  init_info = {};
		init_info.Instance = vulkan_context->GetInstance();
		init_info.PhysicalDevice = vulkan_context->GetPhysicalDevice();
		init_info.Device = vulkan_context->GetLogicalDevice();
		init_info.QueueFamily = vulkan_context->GetGraphicsQueueFamilyIndex();
		init_info.Queue = vulkan_context->GetGraphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = m_DescriptorPool;
		init_info.MinImageCount = 2;
		init_info.ImageCount = static_cast<uint32_t>(vulkan_context->GetSwapchainImages().size());
		init_info.Allocator = nullptr;
		init_info.PipelineInfoMain.Subpass = 0;
		init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.CheckVkResultFn = nullptr;

		init_info.UseDynamicRendering = true;
		VkFormat colorFormat = vulkan_context->GetSwapchainImageFormat();

		VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo{};
		pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		pipelineRenderingInfo.colorAttachmentCount = 1;
		pipelineRenderingInfo.pColorAttachmentFormats = &colorFormat;
		pipelineRenderingInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		pipelineRenderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		init_info.PipelineInfoMain.PipelineRenderingCreateInfo = pipelineRenderingInfo;

		AINE_ASSERT(ImGui_ImplVulkan_Init(&init_info), "Failed to initialize ImGui Vulkan backend");

	}
	void ImGuiLayer::OnUpdate(float dt)
	{
	}
	void ImGuiLayer::OnRender(float dt)
	{
	}
	void ImGuiLayer::OnImGuiRender()
	{
		ImGui::ShowDemoWindow();
	}
	void ImGuiLayer::OnDestroy()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::OnEvent(Event& event)
	{
	}
	void ImGuiLayer::ProcessEvent(const SDL_Event& sdl_event)
	{
		ImGui_ImplSDL3_ProcessEvent(&sdl_event);
	}
	void ImGuiLayer::Begin()
	{

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}
	void ImGuiLayer::End()
	{
		auto& app_instance = Application::GetActiveInstance();
		auto vulkan_context = std::dynamic_pointer_cast<Render::VulkanContext>(app_instance.GetRenderer()->GetContext());
		VkCommandBuffer cmd = vulkan_context->GetCurrentCommandBuffer();
		AINE_ASSERT(cmd != VK_NULL_HANDLE, "ImGuiLayer command buffer is null");

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	}
}