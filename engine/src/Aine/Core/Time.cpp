#include "Core/Time.h"
#include "Core/Log.h"
#include <SDL3/SDL.h>
namespace Aine
{

	float Time::s_DeltaTime = 0.0f;
	float Time::s_TotalTime = 0.0f;
	double  Time::s_LastTime = 0.0f;

	void Time::Init()
	{
		s_DeltaTime = 0.0f;
		s_TotalTime = 0.0f;
		s_LastTime = static_cast<double>(SDL_GetTicksNS()) / 1'000'000'000.0;

		AINE_CORE_INFO("time system initialized !");
	}


	void Time::OnUpdate()
	{
		double currentTime = static_cast<double>(SDL_GetTicksNS()) / 1'000'000'000.0;
		s_DeltaTime = static_cast<float>(currentTime - s_LastTime);
		s_TotalTime += s_DeltaTime;
		s_LastTime = currentTime;
	}


}