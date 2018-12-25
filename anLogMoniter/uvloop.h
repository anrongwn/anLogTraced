#pragma once
#include "..//deps//include//libuv//uv.h"
#include "an_nocopyable.h"
#include <memory>
#include <functional>
#include <stdexcept>

class uvloop : an::NoCopyable {
public:
	explicit uvloop(bool use_default = false)
		: default_loop_(use_default)
		, uv_loop_(use_default ? uv_default_loop() : new uv_loop_t
			, [this](uv_loop_t *loop)
	{
		destroy(loop);
	})
	{
		if (!default_loop_ && uv_loop_init(uv_loop_.get()))
		{
			throw std::runtime_error("uv_loop_init error");
		}
	}

	/**
	*  Destructor
	*/
	~uvloop()
	{
		if (uv_loop_.get())
		{
			uv_loop_close(uv_loop_.get());
		}
	}

	uvloop(uvloop&& other)
		: uv_loop_(std::forward<decltype(other.uv_loop_)>(other.uv_loop_))
	{

	}

	uvloop& operator=(uvloop&& other)
	{
		if (this != &other)
		{
			uv_loop_ = std::forward<decltype(other.uv_loop_)>(other.uv_loop_);
		}
		return *this;
	}

	uv_loop_t* get()
	{
		return uv_loop_.get();
	}

	bool run()
	{
		return uv_run(uv_loop_.get(), UV_RUN_DEFAULT) == 0;
	}

	bool run_once()
	{
		return uv_run(uv_loop_.get(), UV_RUN_ONCE) == 0;
	}

	bool run_nowait()
	{
		return uv_run(uv_loop_.get(), UV_RUN_NOWAIT) == 0;
	}

	
	void update_time()
	{
		uv_update_time(uv_loop_.get());
	}

	
	int64_t now()
	{
		return uv_now(uv_loop_.get());
	}

	
	void stop()
	{
		uv_stop(uv_loop_.get());
	}

private:
	using Deleter = std::function<void(uv_loop_t*)> ;
	void destroy(uv_loop_t *loop) const
	{
		if (!default_loop_)
		{
			delete loop;
		}
	}

	bool default_loop_ = { false };
	std::unique_ptr<uv_loop_t, Deleter> uv_loop_;
};