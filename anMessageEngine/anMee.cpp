#include "stdafx.h"
#include "anMee.h"

#ifdef _DEBUG
#pragma comment(lib, "..//deps//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, "..//deps//lib//libuv//release//libuv.lib")
#endif

std::unique_ptr<uvloop> anMee::s_loop_;
anMee::anMee()
{
}


anMee::~anMee()
{
}

void anMee::start() {

}
void anMee::stop() {
	if (!std::atomic_exchange(&this->flag_, false)) return;

	s_loop_->stop();
}

void anMee::push(const char*data, size_t len) {
	lock lk(mtx_);

	raw_data_q_.emplace(std::string(data, len));
}

anMee::queue::reference anMee::front() {
	lock lk(mtx_);

	return raw_data_q_.front();
}

void anMee::pop() {
	lock lk(mtx_);

	raw_data_q_.pop();
}

void anMee::thread_func(void * lp) {
	anMee * that = static_cast<anMee*>(lp);
	
	if (std::atomic_exchange(&that->flag_, true)) return;
	
	anMee::s_loop_ = std::make_unique<uvloop>(true);

	int more = 0;
	while (that->flag_) {
		if (uv_loop_alive(s_loop_->get())) {

			more = s_loop_->run_once();
			if (more) continue;
		}
		else {
			break;
		}
	}
}