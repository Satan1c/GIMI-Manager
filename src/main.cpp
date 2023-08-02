#include "headers/main.h"

using namespace gimi::vulkan;

int main() {
	auto [code, wd, window, io, err] = cfg();

	if (code != 0) return code;

	bool done = false;
	windows::state = new windows::State(wd, window, &io, &err, &done);
	while (!done) {
		NewFrame(done, window);

		windows::Draw();

		update_main_window(wd);
	}

	shutdown(err, window);
	return 0;
}
