#pragma once
#include "../utility/an_nocopyable.h"
#include "../deps/include/libuv/uv.h"
#include <string>
#include <thread>
#include <atomic>
#include <sstream>
#include <vector>

class anIPC : private an::NoCopyable
{
public:
	using raw_buffer = std::vector<char>;
	anIPC();
	~anIPC();

	int start();
	int stop();
	int open_child_process(const char* path);
	int write(const char level, const char *data, size_t len);

private:
	static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf);
	static void on_write(uv_write_t * req, int status);
	static void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
	static void on_notify(uv_async_t* handle);
	static void on_close(uv_handle_t* handle);
	static void on_walk(uv_handle_t* handle, void* arg);
	static void on_child_process_exit(uv_process_t* process, int64_t exit_status, int term_signal);

	struct an_write : public uv_write_t, private an::NoCopyable {
		uv_buf_t buf;

		explicit an_write(anIPC*host) {
			this->data = host;
		}
		~an_write() {

		}

		an_write() = delete;
	};
	

	static void run(void* arg);
	int init();


	//包长度
	using u_len = union {
		char a[sizeof(size_t)];
		size_t x;
	};
	/**package format
	|lenght|message|
	|*****lenght*****|***level+data***|
	*/
	void pack(const char level, const char *data, size_t len, raw_buffer& message);
	
private:
	uv_loop_t* loop_ = {nullptr};
	uv_process_options_t options_ = {0x00};
	uv_pipe_t stdin_ = {0x00};
	uv_pipe_t stdout_ = {0x00};
	uv_process_t process_ = { 0x00 };
	uv_stdio_container_t stdio_[2];


	std::thread thread_;
	std::atomic_bool flag_ = { ATOMIC_FLAG_INIT }; //是否停止标志
};

