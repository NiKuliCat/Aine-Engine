#pragma once
#include "Render/RendererAPI.h"
namespace Aine::Render
{

	class Renderer
	{
	public:
		Renderer();


	private:
		static GraphicsAPI m_CurrentAPI;
	};
}