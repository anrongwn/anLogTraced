#include "stdafx.h"
#include "anIPCServer.h"


anIPCServer::anIPCServer()
{
	pipe_handler_.data = this;
	message_enginer_ = std::make_unique<anMee>();
	clients_ = std::make_unique<client_contain>(this);

}

anIPCServer::~anIPCServer()
{
	stop();
}

void anIPCServer::on_new_connection(uv_stream_t* server, int status) {
	if (status < 0) {

		uv_close(reinterpret_cast<uv_handle_t*>(server), nullptr);

		g_log->info("anIPCServer::on_new_connection({:08x})={}, errstr={}", \
			(void*)server, status, uv_strerror(status));

		return;
	}

	anIPCServer * that = reinterpret_cast<anIPCServer*>(server->data);

	//生成pipe_client
	std::unique_ptr<uv_pipe_t> client = std::make_unique<uv_pipe_t>();
	int r = 0;
	r = uv_pipe_init(that->loop_, client.get(), 0);
	client->data = that;

	std::string log = fmt::format("anIPCServer::on_new_connection--uv_pipe_init({:08x})={}", (void*)client.get());
	if (r) {
		log += fmt::format(",={}, errstr={}", r, uv_strerror(r));
		g_log->info(log);
		return;
	}

	//accept
	r = uv_accept(server, reinterpret_cast<uv_stream_t*>(client.get()));
	if (0 == r) {
		//read
		r = uv_read_start(reinterpret_cast<uv_stream_t*>(client.get()), \
			anIPCServer::alloc_buffer, anIPCServer::on_read);
		if (r) {
			log += fmt::format(", uv_read_start()={}, errstr={}", r, uv_strerror(r));
			uv_close(reinterpret_cast<uv_handle_t*>(client.get()), NULL);
		}
		else {
			//加入 
			that->clients_->push(std::move(client));
		}
	}

	g_log->info(log);
}

void anIPCServer::alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf) {
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}
void anIPCServer::on_write(uv_write_t * req, int status) {

}
void anIPCServer::on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	std::string log = fmt::format("anIPCServer::on_read({:08x}, {})", (void*)client, nread);
	
	anIPCServer * that = reinterpret_cast<anIPCServer *>(client->data);
	if (nread > 0) {
		that->message_enginer_->push(buf->base, nread);
		log += fmt::format(",data={}", std::string(buf->base, nread));
	}else if (nread < 0) {
		if (!uv_is_closing((uv_handle_t*)client)) {
			uv_close(reinterpret_cast<uv_handle_t*>(client), anIPCServer::on_close);
		}
	}

	g_log->info(log);

	//
	free(buf->base);
}
void anIPCServer::on_notify(uv_async_t* handle) {

}
void anIPCServer::on_close(uv_handle_t* handle) {
	std::string log = fmt::format("anIPCServer::on_close({:08x})", (void*)handle);

	if (UV_ASYNC == handle->type) {
	
	}
	else if (UV_NAMED_PIPE == handle->type) {
		//从连接中清除
		anIPCServer * that = reinterpret_cast<anIPCServer *>(handle->data);
		that->clients_->remove(reinterpret_cast<uv_pipe_t*>(handle));
	}

	g_log->info(log);
}

int anIPCServer::on_message_handle(size_t len, const char* message) {
	int r = 0;
	return r;
}
int anIPCServer::start(uv_loop_t * loop, std::string& serverName) {
	int r = 0;
	loop_ = loop;
	strServerName_ = serverName;

	r = uv_pipe_init(loop_, &pipe_handler_, 0);
	if (r) {
		g_log->info("anIPCServer::start-- uv_pipe_init()={},errstr={}", r, (r ? uv_strerror(r) : "success"));
		return r;
	}

	r = uv_pipe_bind(&pipe_handler_, strServerName_.c_str());
	if (r) {
		g_log->info("anIPCServer::start-- uv_pipe_bind({})={},errstr={}", strServerName_, r, (r ? uv_strerror(r) : "success"));
		return r;
	}

	r = uv_listen(reinterpret_cast<uv_stream_t*>(&pipe_handler_), 128, anIPCServer::on_new_connection);
	if (r) {
		g_log->info("anIPCServer::start-- uv_listen()={},errstr={}", r, (r ? uv_strerror(r) : "success"));
		return r;
	}

	//启动报文分析引擎
	message_enginer_->start(loop_, anIPCServer::on_message_handle);

	return r;
}
int anIPCServer::stop() {
	int r = 0;

	message_enginer_->stop();
	return r;
}
