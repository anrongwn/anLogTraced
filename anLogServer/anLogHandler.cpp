#include "stdafx.h"
#include "anLogHandler.h"


anLogHandler::anLogHandler()
{
}


anLogHandler::~anLogHandler()
{
	close();
}

void anLogHandler::init(const std::string& path, const std::string& logname, \
	const std::string& suffix, int level) {
	if (logger_) return;

	strLogName_ = logname;
	std::string strfullname(path);
	strfullname += "\\";
	strfullname += logname;
	strfullname += suffix;

	//是否已启动日志线程池?
	auto tp = spdlog::thread_pool();
	if (!tp) {
		spdlog::init_thread_pool(65536, 1);
	}
	
	logger_ = spdlog::daily_logger_mt<spdlog::async_factory>(strLogName_, strfullname);

	//logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
	logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
	logger_->set_level(static_cast<spdlog::level::level_enum>(level));
}

void anLogHandler::update_level(const int level) {
	if (logger_) {
		logger_->set_level(static_cast<spdlog::level::level_enum>(level));
	}
}

void anLogHandler::close() {
	if (logger_) {
		logger_->flush();

		spdlog::drop(strLogName_);
	}
}