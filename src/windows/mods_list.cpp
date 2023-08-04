#include "headers/mods_list.hpp"

#include <imgui.h>
#include <map>
#include <string>
#include <archive.h>
#include <archive_entry.h>
//#include "../headers/utils.hpp"
#include "../database/headers/ini_manager.hpp"

namespace windows::mods {
	static std::map<std::string, std::string> mods_map = {};

	void unarchive() {
	}

	void draw() {
		ImGui::Begin("Mods Window");

		if (ImGui::TreeNode("Mods list")) {
			ImGui::TreePop();
		}

		ImGui::End();
	}
}