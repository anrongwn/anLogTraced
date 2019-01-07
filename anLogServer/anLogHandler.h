#pragma once
#include "..//utility/an_nocopyable.h"
#include "..//deps//include/spdlog/spdlog.h"
#include "..//deps//include/spdlog//async.h"
#include "..//deps//include/spdlog/sinks/daily_file_sink.h"
#include "..//deps//include/spdlog/sinks/basic_file_sink.h"
#include "..//deps//include/spdlog//fmt/fmt.h"
#include <memory>
#include <string>

class anLogHandler :
	private an::NoCopyable
{
public:
	anLogHandler();
	~anLogHandler();

	void init(const std::string& path, const std::string& logname, const std::string& suffix, const int level=0);
	void update_level(const int level);
	void close();

	inline std::shared_ptr<spdlog::logger>& instance() {
		return logger_;
	}
	
private:
	std::string strLogName_;
	std::shared_ptr<spdlog::logger> logger_;
};

