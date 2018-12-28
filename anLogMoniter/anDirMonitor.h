#pragma once
#include "../deps//include/libuv/uv.h"
#include "../utility/an_nocopyable.h"
#include <sstream>
#include <string>
#include <functional>
#include <memory>

//
class anDirMonitor;
struct an_fs_event : public uv_fs_event_t {
	explicit an_fs_event(anDirMonitor* monitor) {
		this->data = monitor;
	}
	~an_fs_event() {

	}

	an_fs_event() = delete;
};

struct dmHandler {
	void operator()(uv_fs_event_t* handle, const char* filename, \
		int events, int status);
};

class anDirMonitor : an::NoCopyable
{
public:
	explicit anDirMonitor(uv_loop_t *loop);
	~anDirMonitor();

	int start(const char* path);
	int stop();

private:
	uv_loop_t * loop_ = {nullptr};
	std::unique_ptr<an_fs_event> event_;
	static dmHandler s_handler_;
};

