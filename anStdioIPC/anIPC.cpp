#include "stdafx.h"
#include "anIPC.h"

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif

anIPC::anIPC()
{
}


anIPC::~anIPC()
{
	stop();
}

void anIPC::alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf) {
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}
void anIPC::on_write(uv_write_t * req, int status) {
	an_write *handle = reinterpret_cast<an_write*>(req);
	if (handle->buf.base) {
		free(handle->buf.base);
	}

	delete handle;
}
void anIPC::on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	//std::string log = fmt::format("anIPCServer::on_read({:#08x}, {})", (int)client, nread);

	anIPC * that = reinterpret_cast<anIPC *>(client->data);
	if (nread > 0) {
		
		free(buf->base);
	}
	else if (nread < 0) {
		if (UV_EOF == nread) {
			if (!uv_is_closing((uv_handle_t*)client)) {
				uv_close(reinterpret_cast<uv_handle_t*>(client), anIPC::on_close);
			}
		}
	}

	//g_log->info(log);
}
void anIPC::on_notify(uv_async_t* handle) {

}

void anIPC::on_close(uv_handle_t* handle) {
	//std::string log = fmt::format("anIPCServer::on_close({:#08x})", (int)handle);

	if (UV_ASYNC == handle->type) {

	}
	else if (UV_NAMED_PIPE == handle->type) {
		//从连接中清除
		anIPC * that = reinterpret_cast<anIPC *>(handle->data);
		
	}

	//g_log->info(log);
}

void anIPC::on_walk(uv_handle_t* handle, void* arg) {
	if (!uv_is_closing(handle)) {
		uv_close(handle, nullptr);
	}
}

void anIPC::on_child_process_exit(uv_process_t* process, int64_t exit_status, int term_signal) {

}
int anIPC::init() {
	int r = 0;

	loop_ = uv_default_loop();
	if (nullptr == loop_) {
		return -1;
	}
	uv_loop_set_data(loop_, this);

	r = uv_pipe_init(loop_, &stdin_, 0);
	if (0 == r) {
		uv_handle_set_data((uv_handle_t*)&stdin_, this);
		r = uv_pipe_init(loop_, &stdout_, 0);
		if (0 == r) {
			uv_handle_set_data((uv_handle_t*)&stdout_, this);
		}
	}

	return r;
}
void anIPC::run(void* arg) {
	anIPC * that = reinterpret_cast<anIPC*>(arg);

	int r = 0;

	//初始化
	r = that->init();
	if (r) return;

	//主消息处理
	while (true) {
		if ((that->flag_) && (0 == r))
		{
			break;	//退出标志为true 和没有 io 处理
		}
		r = uv_run(that->loop_, UV_RUN_NOWAIT);
		//g_log->info("anIPCClient::run()--uv_run(UV_RUN_DEFAULT)={}", r);
	}

	//处理还未能关闭的handle，防止内存泄漏
	uv_walk(that->loop_, anIPC::on_walk, nullptr);
	uv_run(that->loop_, UV_RUN_DEFAULT);
	do {
		r = uv_loop_close(that->loop_);
		if (UV_EBUSY == r) {
			uv_run(that->loop_, UV_RUN_NOWAIT);
		}
		//g_log->info("anIPCClient::run() exit. uv_loop_close()={}", r);
	} while (r);
}

int anIPC::start() {
	int r = 0;

	if (thread_.joinable()) return 0;

	thread_ = std::thread(anIPC::run, this);
	if (thread_.joinable()) {
		std::stringstream s;
		s << thread_.get_id();

		//g_log->info("anIPCClient::start() succeeded, threadid={:#08x}", std::stol(s.str()));
	}

	return r;
}

int anIPC::stop() {
	int r = 0;
	
	if (!thread_.joinable()) return 0;

	uv_stop(loop_);

	std::atomic_exchange(&this->flag_, true);


	if (thread_.joinable()) {
		thread_.join();

	}

	return r;
}

int anIPC::open_child_process(const char* path) {
	int r = 0;

	//r = uv_exepath(exepath, &exepath_size);
	//ASSERT(r == 0);
	//exepath[exepath_size] = '\0';
	char* args[3] = { 0x00 };
#ifdef _DEBUG
	args[0] = R"(D:\MyTest\2018_C++\anLogTraced\Debug\anStdioIPC2.exe)";
#else
	args[0] = R"(D:\MyTest\2018_C++\anLogTraced\Release\anStdioIPC2.exe)";
#endif
	args[1] = "anStdioIPC";
	args[2] = NULL;
	options_.file = args[0];
	options_.args = args;
	options_.exit_cb = anIPC::on_child_process_exit;

	options_.stdio = stdio_;
	options_.stdio[0].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_READABLE_PIPE);
	options_.stdio[0].data.stream = (uv_stream_t*)&stdin_;
	options_.stdio[1].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
	options_.stdio[1].data.stream = (uv_stream_t*)&stdout_;
	options_.stdio_count = 2;

	//打开
	r = uv_spawn(loop_, &process_, &options_);
	if (r) {
		return r;
	}

	//开始读
	r = uv_read_start((uv_stream_t*)&stdout_, anIPC::alloc_buffer, anIPC::on_read);


	return r;
}

int anIPC::write(const char level, const char *data, size_t len) {
	int r = 0;
	raw_buffer message;

	if (uv_is_closing((uv_handle_t*)&stdin_)) {
		//g_log->info("anIPCClient::write--pipe is closing");
		return 1;
	}

	pack(level, data, len, message);

	an_write * req = new an_write(this);
	char * buffer = (char*)malloc(message.size());
	req->buf = uv_buf_init(buffer, message.size());
	memcpy(req->buf.base, message.data(), message.size());

	r = uv_write(req, (uv_stream_t*)&stdin_, &req->buf, 1, anIPC::on_write);
	if (r) {
		//free 
		if (req->buf.base) free(req->buf.base);
		delete req;

	}

	return r;
}

/**package format
|lenght|message|
|*****lenght*****|***level+data***|
*/
void anIPC::pack(const char level, const char *data, size_t len, \
	raw_buffer& message) {

	u_len message_len;
	message_len.x = len + sizeof(char);

	//length
	message.insert(message.end(), message_len.a, message_len.a + sizeof(message_len.a));
	//level
	message.push_back(level);
	//data
	message.insert(message.end(), data, data + len);


}