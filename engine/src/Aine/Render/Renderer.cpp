#include "Render/Renderer.h"
#include "Render/VulkanContext.h"
#include "Core/Log.h"
namespace Aine::Render
{

	GraphicsAPI Renderer::s_CurrentAPI = GraphicsAPI::Vulkan;
	Renderer::Renderer(void* windowHandle)
	{
		switch (s_CurrentAPI)
		{
			case GraphicsAPI::Vulkan:
			{
				m_Context = CreateRef<VulkanContext>();
				m_Context->Init(windowHandle);
				break;
			}

			case GraphicsAPI::None:
			case GraphicsAPI::DX12:
			case GraphicsAPI::OpenGL:
			default:
			{
				AINE_CORE_ERROR("GraphicsAPI is not implemented");
				break;
			}
		}

		AINE_CORE_INFO("create renderer sucessfully !");
	}
}