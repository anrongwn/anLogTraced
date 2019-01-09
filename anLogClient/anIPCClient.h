#pragma once
#include "../utility/an_nocopyable.h"
#include "../utility/anwaitconnect.h"
#include "libuv/uv.h"
#include <thread>
#include <atomic>
#include <string>
#include <sstream>
#include <vector>


class anIPCClient : private an::NoCopyable
{
public:
	using raw_buffer = std::vector<char>;
	anIPCClient();
	~anIPCClient();

	int start(const std::string& logname);
	int connect();
	int stop();

	/**
	@param1:level '0'--trace, '1'--debug, '2'--info,\
					'3'--warn, '4'--error, '5'--critical, '6'--off
	@param2:data
	@param3:len
	*/
	int write(const char level, const char *data, size_t len);
public:
	struct an_async : public uv_async_t, private an::NoCopyable {
		uv_buf_t buf;

		explicit an_async(anIPCClient*host) {
			this->data = host;
		}
		~an_async() {

		}

		an_async() = delete;
	};

	struct an_write : public uv_write_t, private an::NoCopyable {
		uv_buf_t buf;

		explicit an_write(anIPCClient*host) {
			this->data = host;
		}
		~an_write() {

		}

		an_write() = delete;
	};
private:
	static void run(void* arg);
	static void on_new_connect(uv_connect_t * req, int status);
	static void on_notify(uv_async_t* handle);
	static void on_close(uv_handle_t* handle);
	static void on_write(uv_write_t * req, int status);

	int init();
	void connect_impl();
	/**package format
	|lenght|message|
	|*****lenght*****|***level+data***|
	*/
	void pack(const char level, const char *data, size_t len, raw_buffer& message);
	int write_pipe(char *data, size_t len);
	//包长度
	using u_len = union {
		char a[sizeof(size_t)];
		size_t x;
	};
private:
	uv_loop_t loop_;
	uv_pipe_t pipe_;
	std::thread thread_;
	uv_connect_t connect_;
	an::an_waitconnect_semaphore wait_;
	uv_process_t process_req_;

	std::atomic_bool flag_ = { ATOMIC_FLAG_INIT }; //是否停止标志
	std::string strlogname_;
};

