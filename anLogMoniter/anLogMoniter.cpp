// anLogMoniter.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "uvloop.h"


#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif


uvloop g_loop(true);
int main()
{
	g_log->info("main starting...");

    return g_loop.run();
}

