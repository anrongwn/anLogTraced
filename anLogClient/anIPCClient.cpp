#include <vld.h>
#include "stdafx.h"
#include "anIPCClient.h"


anIPCClient::anIPCClient()
{
	uv_handle_set_data((uv_handle_t*)&pipe_, this);
	uv_handle_set_data((uv_handle_t*)&loop_, this);
	uv_handle_set_data((uv_handle_t*)&connect_, this);
	//pipe_.data = this;
	//loop_.data = this;
	//connect_.data = this;
}


anIPCClient::~anIPCClient()
{
}
int anIPCClient::init() {
	int r = 0;

	r = uv_loop_init(&loop_);
	if (0 == r) {
		r = uv_pipe_init(&loop_, &pipe_, 0);
	}
	return r;
}

int anIPCClient::start(const std::string& logname) {
	int r = 0;

	if (thread_.joinable()) return r;

	wait_.reset();
	r = this->init();
	if (r) {
		g_log->info("anIPCClient::start--init={}, errstr={}", r, uv_strerror(r));
		return r;
	}

	strlogname_ = logname;
	thread_ = std::thread(anIPCClient::run, this);
	if (thread_.joinable()) {
		std::stringstream s;
		s << thread_.get_id();

		g_log->info("anIPCClient::start() succeeded, threadid={:#08x}", std::stol(s.str()));
	}

	return r;
}

int anIPCClient::stop() {
	int r = 0;

	if (std::atomic_exchange(&this->flag_, true)) return 0;

	/*
	if (!uv_is_closing((uv_handle_t*)&connect_)) {
		uv_close((uv_handle_t*)&connect_, nullptr);
	}
	if (!uv_is_closing((uv_handle_t*)&pipe_)) {
		uv_close((uv_handle_t*)&pipe_, nullptr);
	}
	
	if (uv_is_active((uv_handle_t*)&loop_)) {
		uv_stop(&loop_);
	}
	*/

	if (!uv_is_closing((uv_handle_t*)&pipe_)) {
		uv_close((uv_handle_t*)&pipe_, nullptr);
	}
	uv_stop(&loop_);
	

	if (thread_.joinable()) {
		thread_.join();

		uv_loop_close(&loop_);
		wait_.reset();
	}
	
	return r;
}

#define AN_PIPE_SERVER_NAME_PREFIX	R"(\\?\pipe\)"
void anIPCClient::connect_impl() {
	std::string strPipeName(AN_PIPE_SERVER_NAME_PREFIX);
	strPipeName += strlogname_;
	connect_.data = this;

	uv_pipe_connect(&connect_, &pipe_, strPipeName.c_str(), anIPCClient::on_new_connect);
}
int anIPCClient::connect() {
	int r = 0;

	if (!thread_.joinable()) return 1;	//未能启动 loop

	if (1 == wait_.get_status()) return 0;//已连接
	
	connect_impl();
	wait_.wait();//等待连接完成

	if (1 == wait_.get_status()) {
		r = 0;
	}
	else {
		r = 2;
	}

	return r;
}

void anIPCClient::on_new_connect(uv_connect_t * req, int status) {
	anIPCClient * that = reinterpret_cast<anIPCClient*>( uv_handle_get_data((uv_handle_t*)req) );

	if (status) {
		//启动服务
		int r = that->wait_.get_count();
		if (0 == that->wait_.get_count()) {
			char* args[3];
#ifdef _DEBUG
			args[0] = R"(D:\MyTest\2018_C++\anLogTraced\Debug\anLogServer.exe)";
#else
			args[0] = R"(D:\MyTest\2018_C++\anLogTraced\Release\anLogServer.exe)";
#endif
			args[1] = "anSPTrace";
			args[2] = NULL;

			uv_process_options_t options;
			options.exit_cb = NULL;
#ifdef _DEBUG
			options.cwd  = R"(D:\MyTest\2018_C++\anLogTraced\Debug)";
#else
			options.cwd = R"(D:\MyTest\2018_C++\anLogTraced\Release)";
#endif
			options.args = args;

			options.file = args[0];
			options.flags = UV_PROCESS_DETACHED| UV_PROCESS_WINDOWS_HIDE;
			options.env = nullptr;
			options.stdio_count = 0;

			
			int r = uv_spawn(&that->loop_, &that->process_req_, &options);
			if (r) {
				g_log->info("anIPCClient::on_new_connect--uv_spawn({})={}, errstr={}", options.file, r, uv_strerror(r));
				//return;
			}
			uv_unref((uv_handle_t *)&that->process_req_);
		}
		//::Sleep(10);
		std::this_thread::sleep_for(std::chrono::microseconds(10));

		//重连
		that->connect_impl();
		that->wait_.signal_once();
		
	}
	else if (0 == status) {
		g_log->info("anIPCClient::on_new_connect({:08x}, {}) succeed.", (int)req, status);
		that->wait_.signal();
	}
}

void anIPCClient::on_close(uv_handle_t* handle) {
	if (UV_ASYNC == handle->type) {
		//清除 每次的uv_async_t对象
		an_async *req = reinterpret_cast<an_async*>(handle);
		/*//不释放，待on_write 中释放
		if (req->buf.base) {
			free(req->buf.base);
		}
		*/
		delete req;
	}
	else if (UV_NAMED_PIPE == handle->type) {
		//服务端终止连接后，工作线程uv_run退出，分离当前线程 
		anIPCClient * that = static_cast<anIPCClient*>(uv_handle_get_data((uv_handle_t*)handle));
		if ((&that->pipe_) == (void*)(handle))
		{
			//防此线程退出后，还可以joinable
			that->thread_.detach();
		}

	}

}
void anIPCClient::on_write(uv_write_t * req, int status) {
	//清除 每次的uv_async_t对象
	an_write *handle = reinterpret_cast<an_write*>(req);
	if (handle->buf.base) {
		free(handle->buf.base);
	}

	delete handle;
}
void anIPCClient::run(void * arg) {
	anIPCClient * that = reinterpret_cast<anIPCClient*>(arg);

	int r = 0;
	while (true) {
		if (that->flag_) break;

		r = uv_run(&that->loop_, UV_RUN_NOWAIT);

	}

	g_log->info("anIPCClient::run() exit.");
}
void anIPCClient::on_notify(uv_async_t* handle) {
	an_async *req = static_cast<an_async*>(handle);
	anIPCClient * that = static_cast<anIPCClient*>(uv_handle_get_data((uv_handle_t*)req));
	
	that->write_pipe(req->buf.base, req->buf.len);

	uv_close((uv_handle_t*)handle, anIPCClient::on_close);
}

int anIPCClient::write(const char level, const char *data, size_t len) {
	int r = 0;
	raw_buffer message;

	if (uv_is_closing((uv_handle_t*)&pipe_)) {
		g_log->info("anIPCClient::write--pipe is closing");
		return 1;
	}

	pack(level, data, len, message);

	an_async * req = new an_async(this);
	char * buffer = (char*)malloc(message.size());
	req->buf = uv_buf_init(buffer, message.size());
	memcpy(req->buf.base, message.data(), message.size());

	r = uv_async_init(&loop_, req, anIPCClient::on_notify);
	if (r) {
		g_log->info("anIPCClient::write--uv_async_init={}, errstr={}", r, uv_strerror(r));
	}
	else {
		r = uv_async_send(req);
		if (r) {
			g_log->info("anIPCClient::write--uv_async_send={}, errstr={}", r, uv_strerror(r));
		}
		else {
			g_log->info("anIPCClient::write(level={}, data={}, len={})", level, std::string(message.data(), message.size()), message.size());
		}
	}

	return r;
}

/**package format
|lenght|message|
|*****lenght*****|***level+data***|
*/
void anIPCClient::pack(const char level, const char *data, size_t len, \
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

int anIPCClient::write_pipe(char *data, size_t len) {
	int r = 0;

	if (uv_is_closing((uv_handle_t*)&pipe_)) {
		g_log->info("anIPCClient::write_pipe --pipe is closing");
		return 1;
	}

	an_write * req = new an_write(this);
	uv_handle_set_data((uv_handle_t*)req, this);
	//req->data = this;
	req->buf.base = (data);
	req->buf.len = len;

	r = uv_write((uv_write_t *)req, (uv_stream_t*)&pipe_, \
		&req->buf, 1, anIPCClient::on_write);
	if (r) {
		g_log->info("anIPCClient::uv_write({:08x}, {})={}, errstr={}", (int)req->buf.base, req->buf.len, r, uv_strerror(r));
	}
	else {
		g_log->info("anIPCClient::write_pipe-uv_write(data={}, len={})", std::string(data, len), len);
	}

	return r;
}