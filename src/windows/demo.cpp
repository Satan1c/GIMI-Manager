#include "headers/demo.h"

namespace windows::demo {
	State *state = new State{false};

	void draw() {
		if (!state->show_demo_window) return;

		ImGui::ShowDemoWindow(&state->show_demo_window);
	}
}