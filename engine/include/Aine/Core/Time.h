#pragma once

namespace Aine
{
	class Time
	{
	public:
		
		static void Init();
		static void OnUpdate();


		static float GetDeltaTime() { return s_DeltaTime; }
		static float GetTotalTime() { return s_TotalTime; }



	private:
		static float s_DeltaTime;
		static float s_TotalTime;
		static double  s_LastTime;

	};
}
