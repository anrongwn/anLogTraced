#pragma once

#include "..//deps//include/spdlog/spdlog.h"
#include "..//deps//include/spdlog//async.h"
#include "..//deps//include/spdlog/sinks/daily_file_sink.h"
#include "..//deps//include/spdlog/sinks/basic_file_sink.h"
#include "..//deps//include/spdlog//fmt/fmt.h"
#include <memory>

namespace anlog {
	//const char * logname = R"(anLogMoniter)";
	//const char * logpath = R"(logs/anLogMoniter.log)";
	using logger = std::shared_ptr<spdlog::logger>;
	static logger g_anlog;

	logger& init(const char* logname) {
		if (!g_anlog) {
			std::string logpath(R"(D:\MyTest\2018_C++\anLogTraced\logs\)");
			logpath+=logname;
			logpath+=".log";
			
			//是否已启动日志线程池?
			auto tp = spdlog::thread_pool();
			if (!tp) {
				spdlog::init_thread_pool(65536, 1);
			}
			g_anlog = spdlog::daily_logger_mt<spdlog::async_factory>(logname, logpath);

			g_anlog->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^---%L---%$] [%t] %v");
			g_anlog->set_level(spdlog::level::trace);
			g_anlog->flush_on(spdlog::level::info);
		}
		
		return g_anlog;
	}

	void close(const char* logname) {
		spdlog::drop(logname);
	}

	void set_level(const int level) {
		if (g_anlog) {
			g_anlog->set_level(static_cast<spdlog::level::level_enum>(level));
		}
	}
}

