
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// anLogClient.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


//��־��־
anlog::logger &g_log = anlog::init("anLogClient");
//anlog::logger &g_log = anlog::init("anLogServer");

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif