#include "stdafx.h"
#include "anMee.h"

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif

anMee::anMee():loop_(nullptr)
{
}

anMee::~anMee()
{
	stop();
}

void anMee::start(uv_loop_t* loop, handle_fn cb) {
	if (engine_) return;

	if (nullptr == cb) {
		handler_ = anMee::message_handler;
	}
	else {
		handler_ = cb;
	}

	if (nullptr == loop) {
		loop_ = new uv_loop_t;
		int r = 0;
		r = uv_loop_init(loop_);

		//自起线程处理
		r = uv_thread_create(&engine_, anMee::thread_func, this);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	}
	else {
		loop_ = loop;
	}
}
	
void anMee::stop() {
	if (!std::atomic_exchange(&this->flag_, false)) return;

	//针对自启uv_run
	if ((loop_) && (engine_)) {
		uv_stop(loop_);
		//uv_loop_close(loop_);
		//delete loop_;
		
	}
	uv_thread_join(&engine_);
	loop_ = nullptr;
}

void anMee::push(const char*data, size_t len) {
	lock lk(mtx_);

	raw_data_v_.insert(raw_data_v_.end(), data, data + len);

	g_log->info("anMee::push({})", std::string(data, len));

	emit();
}


void anMee::emit() {
	if (loop_) {
		an_async_req *async = new an_async_req(this);
		uv_async_init(loop_, async, anMee::notify_handler);

		uv_async_send(async);
	}
}

size_t anMee::getRawData(raw_buffer &raw) {
	lock lk(mtx_);

	raw_data_v_.swap(raw);

	return raw.size();
}


#define lv_len sizeof(size_t)
void anMee::notify_handler(uv_async_t* handle) {
	static raw_buffer laster;
	anMee * that = static_cast<anMee*>(handle->data);

	raw_buffer data(laster), tmp;
	size_t raw_len = that->getRawData(tmp);
	laster.clear();
	if (raw_len) {
		data.insert(data.end(), tmp.begin(), tmp.end());
		raw_len = data.size();
	}

	/*//包格式为
		|length|value|
		|4 byte|.....|
	*/
	//获取包实际长度
	while (true) {
		if (raw_len < lv_len) break;

		u_len slen;
		memcpy(&slen.x, data.data(), lv_len);
		
		
		//不是完整的包
		if (slen.x > (raw_len-lv_len)) break;

		//取包
		raw_buffer package;
		package.insert(package.end(), data.begin() + lv_len, data.begin() + lv_len + slen.x);
		data.erase(data.begin(), data.begin() + lv_len + slen.x);
		raw_len = data.size();

		//回调，返回完整的包
		//anMee::message_handler(package.size(), package.data());
		that->handler_(package.size(), package.data());
	}
	
	//保存不完整的包
	laster = std::move(data);

	uv_close((uv_handle_t*)handle, anMee::close_cb);
}
void anMee::close_cb(uv_handle_t* handle) {
	if (handle->type == UV_ASYNC) {
		delete handle;
	}
}
int anMee::message_handler(size_t len, const char* message) {
	int r = 0;

	/*
	@param1:level '0'--trace, '1'--debug, '2'--info,\
					'3'--warn, '4'--error, '5'--critical, '6'--off
					*/
	char level = message[0];

	switch (level){
	case '0':
		g_log->trace("anMee::message_handler(len={}, message={})={}", len, std::string(message+1, len-1), r);
		break;
	case '1':
		g_log->debug("anMee::message_handler(len={}, message={})={}", len, std::string(message + 1, len - 1), r);
		break;
	case '2':
		g_log->info("anMee::message_handler(len={}, message={})={}", len, std::string(message + 1, len - 1), r);
		break;
	case '3':
		g_log->warn("anMee::message_handler(len={}, message={})={}", len, std::string(message + 1, len - 1), r);
		break;
	case '4':
		g_log->error("anMee::message_handler(len={}, message={})={}", len, std::string(message + 1, len - 1), r);
		break;
	case '5':
		g_log->critical("anMee::message_handler(len={}, message={})={}", len, std::string(message + 1, len - 1), r);
		break;
	case '6':
	default:
		break;
	}
	
	return r;
}

void anMee::on_walk(uv_handle_t* handle, void* arg) {
	if (!uv_is_closing(handle)) {
		uv_close(handle, nullptr);
	}
}
void anMee::thread_func(void * lp) {
	anMee * that = static_cast<anMee*>(lp);
	
	if (std::atomic_exchange(&that->flag_, true)) return;
	
	//main run message loop
	int more = 0;
	while (that->flag_) {
		more = uv_run(that->loop_, UV_RUN_NOWAIT);
		if (more) continue;
	}

	//fully close handle
	uv_walk(that->loop_, anMee::on_walk, nullptr);
	uv_run(that->loop_, UV_RUN_DEFAULT);
	do {
		more = uv_loop_close(that->loop_);
		if (UV_EBUSY == more) {
			uv_run(that->loop_, UV_RUN_NOWAIT);
		}
	} while (more);

	//
	delete that->loop_;
}