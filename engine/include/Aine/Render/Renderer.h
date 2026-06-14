#pragma once
#include "Render/RendererAPI.h"
#include "Render/RHIContext.h"
#include "Core/Core.h"
namespace Aine::Render
{

	class Renderer
	{
	public:
		Renderer(void* windowHandle);
		void OnDestroy();

		Ref<RHIContext> GetContext() { return m_Context; }
	private:
		static GraphicsAPI s_CurrentAPI;

		Ref<RHIContext> m_Context = nullptr;
	};
}