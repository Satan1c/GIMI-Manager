#pragma once
#include <imgui.h>

namespace windows::another {
	struct State {
		bool show_another_window;

		State(bool another_window) {
			show_another_window = another_window;
		}
	};

	extern State* state;

	void draw();
}