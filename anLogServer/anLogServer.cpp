// anLogServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <memory>
#include <iostream>
#include <random>
#include "../utility/uvloop2.h"
#include "anIPCServer.h"

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif
#define AN_PIPE_SERVER_NAME_PREFIX	R"(\\?\pipe\)"

//ȫ��uvloop
std::unique_ptr<uvloop2> g_loop = std::make_unique<uvloop2>();

//ȫ��pipe_server
std::unique_ptr<anIPCServer> g_server = std::make_unique<anIPCServer>();


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

	std::string serverName;
#ifdef _DEBUG
	serverName ="anSPTrace";
#else
	serverName = argv[1];
#endif
	
	log = fmt::format("main::logServerName=[{}] started.", serverName);
	g_log->info(log);
	std::cout << log << std::endl;

	//������־������Ϣ
	g_msgHandler.init(R"(D:\MyTest\2018_C++\anLogTraced\logs)", serverName, R"(.ktlog)");

	//����pipe_server
	r = g_server->start(g_loop->get(), std::string(AN_PIPE_SERVER_NAME_PREFIX) + serverName);

	//����uv_run
	r = g_loop->run();

#ifdef _DEBUG
	system("pause");
#endif

    return r;
}

