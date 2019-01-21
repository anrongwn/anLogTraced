// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  在此处引用程序需要的其他头文件
#include "..//deps//include/spdlog/spdlog.h"
#include "..//deps//include/spdlog//async.h"
#include "..//deps//include/spdlog/sinks/daily_file_sink.h"
#include "..//deps//include/spdlog/sinks/basic_file_sink.h"
#include "..//deps//include/spdlog//fmt/fmt.h"
#include <memory>

using logger = std::shared_ptr<spdlog::logger>;
extern logger g_log;
void init_log(const char * logname);
void close_log(const char* logname);