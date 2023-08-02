#pragma once

#include "demo.h"
#include "another.h"
#include "hello_world.h"

namespace windows {

	struct State {
		ImGuiIO *io;
		bool *done;

		State(ImGuiIO *imgui_io, bool *main_done) {
			io = imgui_io;
			done = main_done;
		}
	};

	State *state;

	void Draw() {
		windows::demo::draw();
		windows::another::draw();

		windows::hello_world::draw(state->io->Framerate);
	}
}