#include "stdafx.h"
#include "anMee.h"

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif

//std::shared_ptr<uvloop> anMee::s_loop_;
uvloop *anMee::s_loop_ = new uvloop(true);

anMee::anMee()
{
	notify_.data = this;
}


anMee::~anMee()
{
	stop();
}

void anMee::start() {
	if (engine_) return;

	int r = uv_thread_create(&engine_, anMee::thread_func, this);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
void anMee::stop() {
	if (!std::atomic_exchange(&this->flag_, false)) return;

	s_loop_->stop();
	uv_thread_join(&engine_);
}

void anMee::push(const char*data, size_t len) {
	lock lk(mtx_);

	raw_data_q_.emplace(std::string(data, len));

	g_log->info("anMee::push({})", std::string(data, len));

	emit();
}

anMee::queue::reference anMee::front() {
	lock lk(mtx_);

	return raw_data_q_.front();
}

void anMee::pop() {
	lock lk(mtx_);

	raw_data_q_.pop();
}

void anMee::emit() {
	if (s_loop_->get()) {
		uv_async_init(s_loop_->get(), &notify_, anMee::notify_handler);

		uv_async_send(&notify_);
	}
}

std::string anMee::getRawData() {
	lock lk(mtx_);

	std::stringstream out;
	while (!raw_data_q_.empty()) {
		out << raw_data_q_.front();
		raw_data_q_.pop();
	}

	return out.str();
}

static size_t lv_len = sizeof(size_t);//包长度标识
void anMee::notify_handler(uv_async_t* handle) {
	static std::string laster;
	anMee * that = static_cast<anMee*>(handle->data);

	std::string data;
	data += laster;
	data += that->getRawData();
	laster.clear();

	/*//包格式为
		|length|value|
		|4 byte|.....|
	*/
	//获取包实际长度
	while (true) {
		if (data.length() < lv_len) break;

		std::string slen = data.substr(0, lv_len);
		size_t package_len = std::stoi(slen);
		
		//不是完整的包
		size_t sum_len = data.length();
		if (package_len > sum_len) break;

		//取包
		std::string package = data.substr(lv_len, package_len);
		data.erase(0, lv_len + package_len);

		//回调，返回完整的包
		anMee::message_handler(package.length(), package.c_str());
	}
	
	//保存不完整的包
	laster = std::move(data);
}
int anMee::message_handler(size_t len, const char* message) {
	int r = 0;

	g_log->info("anMee::message_handler(len={}, message={})={}", len, message, r);

	return r;
}
void anMee::thread_func(void * lp) {
	anMee * that = static_cast<anMee*>(lp);
	
	if (std::atomic_exchange(&that->flag_, true)) return;
	
	//anMee::s_loop_ = std::make_shared<uvloop>(true);
	//anMee::s_loop_ = new uvloop(true);

	int more = 0;
	while (that->flag_) {
		more = s_loop_->run_once();
		if (more) continue;
	}

	//delete anMee::s_loop_;
	//anMee::s_loop_ = nullptr;
}