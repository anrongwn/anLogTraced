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

int message_handler(size_t len, const char* message) {
	int r = 0;

	g_log->info("global::message_handler(len={}, message={})={}", len, std::string(message, len), r);
	return r;
}

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

	std::cout << "anMessageEngine start..." << std::endl;
	g_log->info("anMessageEngine start...");

	//������Ϣ��������
	g_engine.start(nullptr, message_handler);

	/*//�����������
	anMee::u_len  len2;
	len2.x = 2;
	std::vector<char> noo;
	noo.insert(noo.end(), len2.a, len2.a + sizeof(size_t));
	noo.insert(noo.end(), 'w');
	g_engine.push(noo.data(), noo.size());
	g_engine.push("a", 1);
	*/
	
	//������뼶���ʱ������   
	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> microClock_type;
	//��ȡ��ǰʱ��㣬windows system_clock��100���뼶���(��ͬϵͳ��һ�����Լ����ս��ܵķ�������)������Ҫת��   
	//microClock_type tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	std::cout << "send thread start..." << std::endl;
	//����ģ�ⷢ����Ϣ�߳�
	std::thread send_thread([]() {
		long count = 0;
		std::random_device rd;
		while (count<2000) {
			std::vector<char> message;

			//��ȡ��ǰʱ��㣬windows system_clock��100���뼶���(��ͬϵͳ��һ�����Լ����ս��ܵķ�������)������Ҫת��   
			microClock_type tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
			std::string body = fmt::format(", ~{}-{},{}~", count, rd(), tp.time_since_epoch().count());
			anMee::u_len len;
			len.x = body.length();
			//memcpy(&len, &tmp, sizeof(int));

			message.insert(message.end(), len.a, len.a + sizeof(size_t));
			message.insert(message.end(), body.begin(), body.end());
			
			g_engine.push(message.data(), message.size());
			++count;

			//
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});
	send_thread.join();

	std::cout << "send thread end." << std::endl;
	system("pause");
	g_engine.stop();
	std::cout << "anMessageEngine end." << std::endl;
	g_log->info("anMessageEngine end.");

    return 0;
}

