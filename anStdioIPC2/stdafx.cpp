// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// anStdioIPC2.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������


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