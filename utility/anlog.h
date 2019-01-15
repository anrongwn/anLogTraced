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
	
	logger& init(const char* logname);

	void close(const char* logname);

	void set_level(const int level);
}

