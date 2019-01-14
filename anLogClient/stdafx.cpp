
// stdafx.cpp : 只包括标准包含文件的源文件
// anLogClient.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


//本志日志
anlog::logger &g_log = anlog::init("anLogClient");
//anlog::logger &g_log = anlog::init("anLogServer");

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif