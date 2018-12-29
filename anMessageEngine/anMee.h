#pragma once
#include "../deps/include/libuv/uv.h"
#include <sstream>
#include <string>
#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <chrono>
#include <numeric>
#include "../utility/uvloop.h"
#include "../utility/an_nocopyable.h"

class anMee : an::NoCopyable
{
public:
	using queue = std::queue<std::string>;
public:
	anMee();
	~anMee();

	void start();
	void stop();
	void push(const char*data, size_t len);
private:
	queue::reference front();
	void pop();
	void emit();
	std::string getRawData();

	//static std::shared_ptr<uvloop> s_loop_;
	static uvloop * s_loop_;
	static void thread_func(void * lp);
	static void notify_handler(uv_async_t* handle);
	static int message_handler(size_t len, const char* message);
private:
	queue raw_data_q_;
	std::mutex mtx_;
	using lock = std::lock_guard<std::mutex>;
	std::atomic_bool flag_ = { ATOMIC_FLAG_INIT };
	uv_thread_t engine_ = {nullptr};

	uv_async_t notify_ = { 0x00 };
};

