#include "headers/another.h"

namespace windows::another {
	State *state = new State{false};

	void draw() {
		if (!state->show_another_window) return;

		ImGui::Begin("Another Window", &state->show_another_window);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me")) state->show_another_window = false;
		ImGui::End();
	}
}