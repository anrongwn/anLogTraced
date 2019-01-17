// anStdioIPC2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <memory>
#include <iostream>
#include <random>
#include "anIPC2.h"


int main(int argc, char *argv[], char *envp[])
{
	/*
	for (auto i = 0; i < argc; ++i) {
	std::string s = fmt::format("argv[{}]={}", i, argv[i]);
	std::cout << s << std::endl;
	}

	for (auto i = 0; nullptr != envp[i]; ++i) {
	std::string s = fmt::format("envp[{}]={}", i, envp[i]);
	std::cout << s << std::endl;
	}
	*/
	if (argc < 2) {
		//log = fmt::format("usage : anLogServer.exe {}", "logservername");
		//g_log->info(log);
		//std::cout << log << std::endl;
#ifndef _DEBUG
		return 0;
#endif
	}


    return 0;
}

