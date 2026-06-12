#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"

#include <string>
namespace Aine
{

	class Log
	{
	public:
		static void Init();
		inline static Ref<spdlog::logger>& GetEngineLogger() { return  s_EngineLogger; }
		inline static Ref<spdlog::logger>& GetAppLogger() { return  s_AppLogger; }


	private:

		static Ref<spdlog::logger> s_EngineLogger;
		static Ref<spdlog::logger> s_AppLogger;
	};



}

//Core Log macros
#define AINE_CORE_TRACE(...)		::Aine::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define AINE_CORE_DEBUG(...)		::Aine::Log::GetEngineLogger()->debug(__VA_ARGS__)
#define AINE_CORE_INFO(...)			::Aine::Log::GetEngineLogger()->info(__VA_ARGS__)
#define AINE_CORE_WARN(...)			::Aine::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define AINE_CORE_ERROR(...)		::Aine::Log::GetEngineLogger()->error(__VA_ARGS__)
#define AINE_CORE_CRITICAL(...)		::Aine::Log::GetEngineLogger()->critical(__VA_ARGS__)


// Clent Log macros
#define AINE_CLENT_TRACE(...)				::Aine::Log::GetAppLogger()->trace(__VA_ARGS__)
#define AINE_CLENT_DEBUG(...)				::Aine::Log::GetAppLogger()->debug(__VA_ARGS__)
#define AINE_CLENT_INFO(...)				::Aine::Log::GetAppLogger()->info(__VA_ARGS__)
#define AINE_CLENT_WARN(...)				::Aine::Log::GetAppLogger()->warn(__VA_ARGS__)
#define AINE_CLENT_ERROR(...)				::Aine::Log::GetAppLogger()->error(__VA_ARGS__)
#define AINE_CLENT_CRITICAL(...)			::Aine::Log::GetAppLogger()->critical(__VA_ARGS__)