// stdafx.cpp : 只包括标准包含文件的源文件
// anLogServer.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

//本志日志
anlog::logger &g_log = anlog::init("anLogServer");

//日志服务处理
anLogHandler g_msgHandler;
