#pragma once

#include "modals.hpp"
#include "mods_zip_list.hpp"
#include "file_move_test.hpp"
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
		windows::zips::draw_list();
		windows::move_test::draw();
		windows::demo::draw();
		windows::another::draw();

		windows::hello_world::draw();
	}
}