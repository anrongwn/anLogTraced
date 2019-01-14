#pragma once
#include "..//utility/an_nocopyable.h"
#include "../deps/include/libuv/uv.h"
#include "../anMessageEngine/anMee.h"
#include "anLogHandler.h"
#include <string>
#include <memory>
#include <list>
#include <functional>

class anIPCServer;
class client_contain :private an::NoCopyable {
public:
	using client_item = std::unique_ptr<uv_pipe_t>;

	client_contain() = delete;
	explicit client_contain(class anIPCServer* host) :host_(host){

	}
	~client_contain() {
	}
	
	void push(client_item&& client) {
		list_.push_back(std::forward<client_item>(client));
	}

	void remove(uv_pipe_t *handle) {
		list_.remove_if([&](std::list<client_item>::reference ref) {
			return (ref.get() == handle); });
	}

	size_t get_count() {
		return list_.size();
	}
private:
	std::list<client_item> list_;
	class anIPCServer * host_ = { nullptr };
};

class anIPCServer :
	private an::NoCopyable
{
public:
	anIPCServer();
	~anIPCServer();

	int start(std::string&& serverName);
	int run();
	int wait_exit();
private:
	int stop();

	static void on_new_connection(uv_stream_t* server, int status);
	static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf);
	static void on_write(uv_write_t * req, int status);
	static void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
	static void on_notify(uv_async_t* handle);
	static void on_close(uv_handle_t* handle);

	static int on_message_handle(size_t len, const char* message);
private:
	std::string strServerName_;
	uv_loop_t * loop_ = { nullptr };
	uv_pipe_t  pipe_server_ = { 0x00 };
	

	std::unique_ptr<anMee> message_enginer_;
	std::unique_ptr<client_contain> clients_;
	
};

