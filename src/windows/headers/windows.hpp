#pragma once

#include "modals.hpp"
#include "mods_zip_list.hpp"
#include "mods_list.hpp"
#include "demo.hpp"
#include "another.hpp"
#include "hello_world.hpp"

namespace windows {

	struct State {
		bool *done;

		explicit State(bool *main_done) {
			done = main_done;
		}
	};

	State *state;

	void Draw() {
		windows::mods::draw();
		windows::zips::draw();
		windows::demo::draw();
		windows::another::draw();

		windows::hello_world::draw();
	}
}