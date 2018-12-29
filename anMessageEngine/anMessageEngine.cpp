// anMessageEngine.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "anMee.h"
#include <iostream>
#include <random>

anMee g_engine;


std::stringstream& operator >> (std::stringstream& is, std::string& data)
{
	while (!is.eof()) {
		char c = is.get();
		if (c == -1) break;

		data += c;
	}
	
	return is;
}


int main()
{
	size_t int_l = sizeof(int);
	char * p = new char[int_l];
	int an = 1000;
	/*
	p[0] = (char)(an & 0xff);
	p[1] = (char)((an>>8) & 0xff);
	p[2] = (char)((an>>16) & 0xff);
	p[3] = (char)((an>>24) & 0xff);
	*/
	
	memcpy(p, &an, int_l);
	const char * pnm = "1000";
	int aa = 0;
	memcpy(&aa, p, int_l);
	
	delete []p;

	std::string dad("an, and");
	std::stringstream ss;
	ss.unsetf(std::ios::skipws);
	ss << dad;
	std::string dac;
	//ss >> std::noskipws>>dac;
	
	ss >> dac;
	std::string daf;
	ss >> daf;
	std::cout << "anMessageEngine start..." << std::endl;
	g_log->info("anMessageEngine start...");

	//������Ϣ��������
	g_engine.start();

	//������뼶���ʱ������   
	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> microClock_type;
	//��ȡ��ǰʱ��㣬windows system_clock��100���뼶���(��ͬϵͳ��һ�����Լ����ս��ܵķ�������)������Ҫת��   
	//microClock_type tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	std::cout << "send thread start..." << std::endl;
	//����ģ�ⷢ����Ϣ�߳�
	std::thread send_thread([]() {
		long count = 0;
		std::random_device rd;
		while (count<20) {
			std::stringstream message;

			//��ȡ��ǰʱ��㣬windows system_clock��100���뼶���(��ͬϵͳ��һ�����Լ����ս��ܵķ�������)������Ҫת��   
			microClock_type tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
			std::string body = fmt::format(", ~{}-{},{}~", count, rd(), tp.time_since_epoch().count());
			message << body.length();
			message << body;
			
			g_engine.push(message.str().c_str(), message.str().length());
			++count;

			//
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});
	send_thread.join();

	std::cout << "send thread end." << std::endl;
	system("pause");

    return 0;
}

