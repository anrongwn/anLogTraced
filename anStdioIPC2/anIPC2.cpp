#include "stdafx.h"
#include "anIPC2.h"


#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif


anIPC2::anIPC2()
{
}


anIPC2::~anIPC2()
{
}

void anIPC2::alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf) {
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}
void anIPC2::on_write(uv_write_t * req, int status) {
	an_write *handle = reinterpret_cast<an_write*>(req);
	if (handle->buf.base) {
		free(handle->buf.base);
	}

	delete handle;
}
void anIPC2::on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	std::string log = fmt::format("anIPC2::on_read({:#08x}, {})", (int)client, nread);

	anIPC2 * that = reinterpret_cast<anIPC2 *>(client->data);
	if (nread > 0) {
		log += fmt::format(", data={}", std::string(buf->base, nread));

		int r = that->write(0x32, buf->base, nread);
		log += fmt::format(", echo={}", r);

		free(buf->base);
	}
	else if (nread < 0) {
		if (UV_EOF == nread) {
			if (!uv_is_closing((uv_handle_t*)client)) {
				uv_close(reinterpret_cast<uv_handle_t*>(client), anIPC2::on_close);
			}
		}
	}

	g_log->info(log);
}
void anIPC2::on_notify(uv_async_t* handle) {

}

void anIPC2::on_close(uv_handle_t* handle) {
	//std::string log = fmt::format("anIPCServer::on_close({:#08x})", (int)handle);

	if (UV_ASYNC == handle->type) {

	}
	else if (UV_NAMED_PIPE == handle->type) {
		//从连接中清除
		anIPC2 * that = reinterpret_cast<anIPC2 *>(handle->data);

	}

	//g_log->info(log);
}

void anIPC2::on_walk(uv_handle_t* handle, void* arg) {
	if (!uv_is_closing(handle)) {
		uv_close(handle, nullptr);
	}
}

int anIPC2::init() {
	int r = 0;
	std::string log("anIPC2::init()-");

	loop_ = uv_default_loop();
	if (nullptr == loop_) {
		return -1;
	}
	uv_loop_set_data(loop_, this);

	r = uv_pipe_init(loop_, &stdin_, 0);
	log += fmt::format("uv_pipe_init(stdin_{:#08x})={}, errstr={}", (int)&stdin_, r, (r?uv_strerror(r):"sucess"));
	if (0 == r) {
		uv_handle_set_data((uv_handle_t*)&stdin_, this);
		r = uv_pipe_open(&stdin_, 0);
		log += fmt::format(",uv_pipe_open(stdin_{:#08x})={}, errstr={}", (int)&stdin_, r, (r ? uv_strerror(r) : "sucess"));

		r = uv_pipe_init(loop_, &stdout_, 0);
		log += fmt::format(",uv_pipe_init(stdout_{:#08x})={}, errstr={}", (int)&stdout_, r, (r ? uv_strerror(r) : "sucess"));
		if (0 == r) {
			r = uv_pipe_open(&stdout_, 1);
			log += fmt::format(",uv_pipe_open(stdout_{:#08x})={}, errstr={}", (int)&stdout_, r, (r ? uv_strerror(r) : "sucess"));
			uv_handle_set_data((uv_handle_t*)&stdout_, this);
		}
	}

	g_log->info(log);

	return r;
}

int anIPC2::read_start() {
	int r = 0;

	r = uv_read_start((uv_stream_t*)&stdin_, anIPC2::alloc_buffer, anIPC2::on_read);

	return r;
}
void anIPC2::run(void* arg) {
	anIPC2 * that = reinterpret_cast<anIPC2*>(arg);

	int r = 0;

	//初始化
	r = that->init();
	if (r) return;

	//开始读
	r = that->read_start();
	
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
	uv_walk(that->loop_, anIPC2::on_walk, nullptr);
	uv_run(that->loop_, UV_RUN_DEFAULT);
	do {
		r = uv_loop_close(that->loop_);
		if (UV_EBUSY == r) {
			uv_run(that->loop_, UV_RUN_NOWAIT);
		}
		//g_log->info("anIPCClient::run() exit. uv_loop_close()={}", r);
	} while (r);
}

int anIPC2::start() {
	int r = 0;

	if (thread_.joinable()) return 0;

	thread_ = std::thread(anIPC2::run, this);
	if (thread_.joinable()) {
		std::stringstream s;
		s << thread_.get_id();

		//g_log->info("anIPCClient::start() succeeded, threadid={:#08x}", std::stol(s.str()));
	}

	return r;
}

int anIPC2::stop() {
	int r = 0;

	if (!thread_.joinable()) return 0;

	uv_stop(loop_);

	std::atomic_exchange(&this->flag_, true);


	if (thread_.joinable()) {
		thread_.join();

	}

	return r;
}

int anIPC2::write(const char level, const char *data, size_t len) {
	int r = 0;
	raw_buffer message;

	if (uv_is_closing((uv_handle_t*)&stdout_)) {
		//g_log->info("anIPCClient::write--pipe is closing");
		return 1;
	}

	pack(level, data, len, message);

	an_write * req = new an_write(this);
	char * buffer = (char*)malloc(message.size());
	req->buf = uv_buf_init(buffer, message.size());
	memcpy(req->buf.base, message.data(), message.size());

	r = uv_write(req, (uv_stream_t*)&stdout_, &req->buf, 1, anIPC2::on_write);
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
void anIPC2::pack(const char level, const char *data, size_t len, \
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