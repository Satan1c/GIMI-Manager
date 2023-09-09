#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "headers/main.hpp"

int main() {
#if SDL2_VULKAN
	auto [code, wd, window, io, err] = cfg();
#elif WINDOWS_NATIVE
	auto [code, io, wc, hwnd] = cfg();
#endif

	if (code != 0) return code;

	bool done = false;
	//windows::state = new windows::State(wd, window, &io, &err, &done);
	database::ini_manager::init();
	windows::zips::reload_list();
	//windows::mods::reload_list();
	windows::state = new windows::State(&done);
	while (!done) {
#if SDL2_VULKAN
		gimi::vulkan::NewFrame(done, window);
#elif WINDOWS_NATIVE
		gimi::dx11::NewFrame(done);
#endif
		if (done)
			break;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		windows::Draw();
#if SDL2_VULKAN
		update_main_window(wd);
#elif WINDOWS_NATIVE
		update_main_window();
#endif

	}

#if SDL2_VULKAN
	shutdown(err, window);
#elif WINDOWS_NATIVE
	shutdown(hwnd, wc);
#endif

	return 0;
}
