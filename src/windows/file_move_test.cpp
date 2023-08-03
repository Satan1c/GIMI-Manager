#include "headers/file_move_test.hpp"
#include "headers/mods_zip_list.hpp"
#include <string>
#include <filesystem>
#include <imgui.h>
#include <imgui_stdlib.h>

namespace windows::move_test {
	std::string from = R"(C:\Games\Modded genshin\Mods zips)";

	void draw() {
		ImGui::Begin("Copy test Window");
		ImGui::InputText("Copy from", &from);

		if (ImGui::BeginPopupModal("####From is empty")) {
			ImGui::Text("From path can't be empty");

			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		if (ImGui::Button("Copy")) {
			if (!from.empty()) {
				windows::zips::zips_map.clear();
				for (const auto &entry: std::filesystem::recursive_directory_iterator(from)) {
					if (!entry.is_regular_file()) continue;

					try {
						const auto &path = entry.path();
						auto key_str = path.filename().string();
						auto value_str = path.string();

						auto key = std::string(key_str);
						auto value = std::string(value_str);

						windows::zips::zips_map[key] = value;
					} catch (const std::filesystem::filesystem_error &) {
					} catch (const std::exception &) {
					}
				}

				windows::zips::state = true;
			} else {
				ImGui::OpenPopup("####From is empty");
			}
		}

		ImGui::End();
	}
}