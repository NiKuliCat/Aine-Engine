#pragma once

namespace Aine::Render
{
	enum class GraphicsAPI
	{
		None = 0,
		OpenGL,
		Vulkan,
		DX12,
	};


	struct RendererConfig
	{
		GraphicsAPI	m_API;
	};
}