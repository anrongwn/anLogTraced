// stdafx.cpp : 只包括标准包含文件的源文件
// anStdioIPC2.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用


logger g_log;

void init_log(const char* logname) {
	if (!g_log) {
		std::string logpath(R"(D:\MyTest\2018_C++\anLogTraced\logs\)");
		logpath += logname;
		logpath += ".log";

		g_log = spdlog::daily_logger_mt<spdlog::default_factory>(logname, logpath);

		g_log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^---%L---%$] [%t] %v");
		g_log->set_level(spdlog::level::trace);
		g_log->flush_on(spdlog::level::info);
	}
}

void close_log(const char* logname) {
	spdlog::drop(logname);
}