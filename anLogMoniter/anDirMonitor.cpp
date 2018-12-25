#include "stdafx.h"
#include "anDirMonitor.h"


dmHandler anDirMonitor::s_handler_;
anDirMonitor::anDirMonitor(uv_loop_t *loop) : loop_(loop)
{
	event_ = std::make_unique<an_fs_event>(this);
}


anDirMonitor::~anDirMonitor()
{
}

int anDirMonitor::start(const char* path) {
	int r = 0;
	std::stringstream log;
	log << "anDirMonitor::start ";

	r = uv_fs_event_init(loop_, event_.get());
	if (r) {
		log << fmt::format("--uv_fs_event_init()={},{}", r, r?uv_strerror(r):"");
	}
	else {
		//std::function<void(uv_fs_event_t* , const char* ,int , int)> f = handler_;
		
		r = uv_fs_event_start(event_.get(), [](uv_fs_event_t* handle, const char* filename, \
			int events, int status) {
			anDirMonitor::s_handler_(handle, filename, events, status);
		}, path, UV_FS_EVENT_STAT);

		log << fmt::format(", --uv_fs_event_start()={},{}", r, r ? uv_strerror(r) : "");
	}

	g_log->info("{}", log.str());

	return r;
}

int anDirMonitor::stop() {
	int r = 0;

	if (event_.get())
		r = uv_fs_event_stop(event_.get());

	return r;
}