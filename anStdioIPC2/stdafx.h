// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
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