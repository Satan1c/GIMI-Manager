#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "headers/main.h"

int main() {
	//auto [code, wd, window, io, err] = cfg();
	auto [code, io, wc, hwnd] = cfg();

	if (code != 0) return code;

	bool done = false;
	//windows::state = new windows::State(wd, window, &io, &err, &done);
	windows::state = new windows::State(&io, &done);
	while (!done) {
		//gimi::vulkan::NewFrame(done, window);
		gimi::dx11::NewFrame(done);

		if (done)
			break;

		windows::Draw();

		update_main_window();
	}

	shutdown(hwnd, wc);
	return 0;
}
