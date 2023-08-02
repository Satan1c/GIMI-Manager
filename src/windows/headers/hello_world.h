#pragma once

#include <imgui.h>

namespace windows::hello_world {
	struct State {
		bool* show_demo_window;
		bool* show_another_window;
		ImVec4 clear_color;

		State(bool* demo_window, bool* another_window, ImVec4 color) {
			show_demo_window = demo_window;
			show_another_window = another_window;
			clear_color = color;
		}
	};

	extern State* state;

	void draw(const float &frmerate);
}