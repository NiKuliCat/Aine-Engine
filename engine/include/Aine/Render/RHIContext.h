#pragma once
#include <cstdint>
namespace Aine::Render
{
	class RHIContext
	{
	public:
		virtual ~RHIContext() = default;

		virtual void Init(void* windowHandle) = 0;

		virtual void OnDestroy() = 0;
		virtual void OnResize(uint32_t width, uint32_t height) = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;



	};
}