#pragma once
#include "../deps/include/libuv/uv.h"
#include "../utility/an_nocopyable.h"
#include <memory>

struct uvloop2 : an::NoCopyable {
	uvloop2() {
		loop_ = uv_default_loop();
	}

	~uvloop2() {
		close();
	}
	uv_loop_t* get() {
		return loop_;
	}

	int close() {
		if ((loop_) && (uv_loop_alive(loop_))) {
			return uv_loop_close(loop_);
		}
		else {
			return 0;
		}
	}

	void stop() {
		if (loop_) {
			uv_stop(loop_);
		}
	}

	int run()
	{
		return uv_run(loop_, UV_RUN_DEFAULT);
	}

	int run_once()
	{
		return uv_run(loop_, UV_RUN_ONCE);
	}

	int run_nowait()
	{
		return uv_run(loop_, UV_RUN_NOWAIT);
	}


	void update_time()
	{
		uv_update_time(loop_);
	}


	int64_t now()
	{
		return uv_now(loop_);
	}

private:
	uv_loop_t * loop_ = {nullptr};
};