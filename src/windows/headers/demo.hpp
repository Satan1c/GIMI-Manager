#pragma once

namespace windows::demo {
	struct State {
		bool show_demo_window;

		State(bool demo_window) {
			show_demo_window = demo_window;
		}
	};

	extern State *state;

	void draw();
}