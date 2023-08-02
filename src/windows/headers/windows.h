#pragma once

#include "demo.h"
#include "another.h"
#include "hello_world.h"

namespace windows {
	struct State {
		ImGui_ImplVulkanH_Window * wd;
		SDL_Window * window;
		ImGuiIO* io;
		VkResult* err;
		bool* done;

		State(ImGui_ImplVulkanH_Window * vulkan_wd,
				SDL_Window * sdl_window,
				ImGuiIO * imgui_io,
				VkResult * vulkan_err,
				bool * main_done) {
			wd = vulkan_wd;
			window = sdl_window;
			io = imgui_io;
			err = vulkan_err;
			done = main_done;
		}
	};

	State* state;

	void Draw() {
		windows::demo::draw();
		windows::another::draw();

		windows::hello_world::draw(state->io->Framerate);
	}
}