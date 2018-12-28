// anLogMoniter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "../utility/uvloop.h"
#include "anDirMonitor.h"


#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif


uvloop g_loop(true);
int main()
{
	g_log->info("main starting...");

	anDirMonitor dm(g_loop.get());
	dm.start(R"(D:\MyTest\2018_C++\anLogTraced\anLogMoniter\logs)");
	
    return g_loop.run();
}

