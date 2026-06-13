#include "Core/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Aine
{

	Ref<spdlog::logger> Log::s_EngineLogger;
	Ref<spdlog::logger> Log::s_AppLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_EngineLogger = spdlog::stdout_color_mt("ENGINE");
		s_EngineLogger->set_level(spdlog::level::trace);

		s_AppLogger = spdlog::stderr_color_mt("EDITOR");
		s_AppLogger->set_level(spdlog::level::trace);

		AINE_CORE_INFO("log system initialized !");
	}
}