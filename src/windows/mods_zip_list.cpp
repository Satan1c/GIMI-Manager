#include "headers/mods_zip_list.hpp"

#include <vector>
#include <imgui.h>
#include <imgui_stdlib.h>

namespace windows::zips {
	bool state = false;
	std::map<std::string, std::string> zips_map = std::map < std::string, std::string
	>();

	static std::string m_selected = "";
	static std::string m_selected_path = "";

	std::filesystem::path modal_file;
	std::string file_extension;
	std::string mod_name;
	std::string file_name;

	std::filesystem::path directory = R"(C:\Users\Satan1c\Desktop\test\Zips)";
	std::filesystem::copy_options options =
			std::filesystem::copy_options::recursive
			| std::filesystem::copy_options::update_existing;

	void grab_dirs(const std::filesystem::path &dir) {
		for (const auto &entry: std::filesystem::directory_iterator(dir)) {
			if (!entry.is_directory()) continue;
			auto name = std::string(entry.path().filename().string());

			try {
				if (std::count_if(
						std::filesystem::directory_iterator(entry.path()),
						std::filesystem::directory_iterator{},
						(bool (*)(const std::filesystem::path &)) std::filesystem::is_directory) < 1) {
					auto flags =
							ImGuiTreeNodeFlags_Leaf
							| ImGuiTreeNodeFlags_NoTreePushOnOpen
							| (name == m_selected ? ImGuiTreeNodeFlags_Selected : 0);
					ImGui::TreeNodeEx(name.c_str(), flags);
					if (ImGui::IsItemClicked()) {
						m_selected = name;
						m_selected_path = entry.path().string();
					}
					continue;
				}
				if (ImGui::TreeNode(name.c_str())) {
					grab_dirs(entry.path());

					ImGui::TreePop();
				}
			} catch (const std::filesystem::filesystem_error &) {
			} catch (const std::exception &) {
			}
		}
	}

	void draw_list() {
		if (!state) return;

		static bool modal = false;
		ImGui::Begin("Zips Window");

		if (ImGui::TreeNode("Zips list")) {
			for (const auto &pair: zips_map) {
				if (ImGui::TreeNode(pair.first.c_str())) {

					ImGui::Text("%s", pair.second.c_str());

					if (ImGui::SmallButton("unarchive")) {
						modal = true;
						modal_file = pair.second;
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		if (modal) {
			ImGui::OpenPopup("Choose path");

			if (ImGui::BeginPopupModal("Choose path")) {
				if (ImGui::TreeNode("Category list")) {
					grab_dirs(directory);
					ImGui::TreePop();
				}

				ImGui::Spacing();

				if (file_extension.empty())
					file_extension = std::string(modal_file.extension().string());
				if (file_name.empty())
					file_name = std::string(modal_file.filename().replace_extension().string());

				ImGui::Text("Mod name:");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::BeginItemTooltip()) {
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Under that dir name will store all files that relative to that mod");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::InputText("##", &mod_name);

				ImGui::Text("Filename name:");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::BeginItemTooltip()) {
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Rename selected archive file");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::InputText("###", &file_name);

				ImGui::Spacing();
				if (ImGui::Button("Choose")) {
					auto from = modal_file;
					auto mod_path = std::filesystem::path(m_selected_path + '\\' + mod_name);
					if (!exists(mod_path))
						std::filesystem::create_directory(mod_path);
					auto to = mod_path.string() + '\\' + file_name + file_extension;
					std::filesystem::copy(from, to, options);
					//std::filesystem::remove(modal_file);
					modal = false;

					auto *clear = "";
					m_selected = clear;
					m_selected_path = clear;
					file_extension = clear;
					mod_name = clear;
					file_name = clear;

					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}