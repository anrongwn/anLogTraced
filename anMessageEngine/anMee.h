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
#include <vector>
#include "../utility/an_nocopyable.h"


class anMee : an::NoCopyable
{
public:
	using raw_buffer = std::vector<char>;
	//包长度
	using u_len = union {
		char a[sizeof(size_t)];
		size_t x;
	};
	using handle_fn = int(*)(size_t, const char*);

private:
	struct an_async_req : public uv_async_t {
		explicit an_async_req(class anMee *p) {
			this->data = p;
		}
		an_async_req() = delete;
		~an_async_req() {

		}
	};

public:
	anMee();
	~anMee();
	
	/**
	@loop:	=nullptr, 自创loop及uv_run线程
			！=nullptr, 使用loop 及 loop所在的uv_run线程
	@cb:	处理器
	*/
	void start(uv_loop_t* loop, handle_fn cb);
	void stop();
	void push(const char*data, size_t len);
private:
	void emit();
	size_t getRawData(raw_buffer &raw);

	static void thread_func(void * lp);
	static void notify_handler(uv_async_t* handle);
	static int message_handler(size_t len, const char* message);
	static void close_cb(uv_handle_t* handle);
	static void on_walk(uv_handle_t* handle, void* arg);
private:
	raw_buffer raw_data_v_;

	std::mutex mtx_;
	using lock = std::lock_guard<std::mutex>;
	std::atomic_bool flag_ = { ATOMIC_FLAG_INIT };
	uv_thread_t engine_ = {nullptr};

	std::function<int(size_t, const char*)> handler_;
	uv_loop_t * loop_;
};

