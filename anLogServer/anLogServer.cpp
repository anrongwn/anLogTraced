// anLogServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <memory>
#include <iostream>
#include <random>
#include "../utility/uvloop2.h"

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif

//全局uvloop
std::unique_ptr<uvloop2> g_loop = std::make_unique<uvloop2>();
int main(int argc, char *argv[], char *envp[])
{
	int r = 0;
	std::string log;
	
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
		log = fmt::format("usage : anLogServer.exe {}", "logservername");
		g_log->info(log);
		std::cout << log << std::endl;
#ifndef _DEBUG
		return 0;
#endif
	}
#ifdef _DEBUG
	std::string serverName ="anLogServer";
#else
	std::string serverName = argv[1];
#endif
	
	log = fmt::format("main::logServerName=[{}] started.", serverName);
	g_log->info(log);
	std::cout << log << std::endl;


	//开启uv_run
	r = g_loop->run();

#ifdef _DEBUG
	system("pause");
#endif

    return r;
}

