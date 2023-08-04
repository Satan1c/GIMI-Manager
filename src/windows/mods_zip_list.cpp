#include "headers/mods_zip_list.hpp"

#include <map>
#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <filesystem>
#include "../database/headers/ini_manager.hpp"
#include "../headers/utils.hpp"

namespace windows::zips {
	static std::map<std::string, std::string> zips_map = {};

	static bool modal = false;

	static std::map<std::string, std::string *> m_selected_files = {};
	static std::string m_selected;
	static std::string m_selected_path;

	static std::filesystem::path modal_file;
	static std::string file_extension;
	static std::string mod_name;
	static std::string file_name;

	static std::filesystem::copy_options options =
			std::filesystem::copy_options::recursive
			| std::filesystem::copy_options::update_existing;

	void close_choose() {
		modal = false;

		m_selected.clear();
		m_selected_path.clear();
		mod_name.clear();
		file_name.clear();
		file_extension.clear();

		ImGui::CloseCurrentPopup();
	}

	void move_file(const std::filesystem::path &from, const std::filesystem::path &to) {
		if (!exists(to))
			std::filesystem::create_directory(to);
		std::filesystem::copy(
				from,
				to.string() + '\\' + file_name + file_extension,
				options);
		//std::filesystem::remove(modal_file);
	}

	void draw_dirs(const std::filesystem::path &dir, std::string &selected, std::string &selected_path,
				   const bool &empty = true) {
		for (const auto &entry: std::filesystem::directory_iterator(dir)) {
			if (!entry.is_directory()) continue;

			auto name = std::string(entry.path().filename().string());
			auto flags = ImGuiTreeNodeFlags_SpanAvailWidth
						 | ImGuiTreeNodeFlags_OpenOnArrow
						 | ImGuiTreeNodeFlags_OpenOnDoubleClick
						 | (selected == name
							? ImGuiTreeNodeFlags_Selected
							: 0);

			try {
				auto count = std::count_if(
						std::filesystem::directory_iterator(entry.path()),
						std::filesystem::directory_iterator{},
						(bool (*)(const std::filesystem::path &)) std::filesystem::is_directory);
				if (empty ? count < 1 : count > 0) {


					ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf
													| ImGuiTreeNodeFlags_NoTreePushOnOpen
													| flags);

					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
						selected = name;
						selected_path = entry.path().string();
					}
				} else if (ImGui::TreeNodeEx(name.c_str(), flags)) {
					draw_dirs(entry.path(), selected, selected_path);
					ImGui::TreePop();
				} else {
					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
						selected = name;
						selected_path = entry.path().string();
					}
				}
			} catch (const std::filesystem::filesystem_error &) {
			} catch (const std::exception &) {
			}
		}
	}

	void reload_list() {
		gimi::utils::load_files(database::ini_manager::downloads_folder(), zips_map);
	}

	void draw() {
		ImGui::Begin("Zips Window");

		if (ImGui::SmallButton("Reload")) {
			reload_list();
		}

		if (ImGui::SmallButton("Move")) {
			modal = true;
		}

		if (ImGui::TreeNode("Zips list")) {
			for (const auto &pair: zips_map) {
				auto is_selected = gimi::utils::GetWithDefp(m_selected_files, pair.first, (std::string *) nullptr);
				if (ImGui::TreeNodeEx(pair.first.c_str(),
									  ImGuiTreeNodeFlags_Leaf
									  | ImGuiTreeNodeFlags_NoTreePushOnOpen
									  | ImGuiTreeNodeFlags_SpanAvailWidth
									  | ImGuiTreeNodeFlags_OpenOnDoubleClick
									  | (is_selected
										 ? ImGuiTreeNodeFlags_Selected
										 : 0))) {

					if (ImGui::IsItemClicked()) {
						m_selected_files[pair.first] = is_selected ? (std::string *) nullptr
																   : (std::string *) (&(pair.second));
					}
				}
			}
			ImGui::TreePop();
		}

		if (modal) {
			ImGui::OpenPopup("Choose path");

			if (ImGui::BeginPopupModal("Choose path")) {
				if (ImGui::TreeNode("Category list")) {
					draw_dirs(database::ini_manager::zips_folder(), m_selected, m_selected_path);
					ImGui::TreePop();
				}

				ImGui::Spacing();

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

				ImGui::Spacing();

				if (!m_selected.empty()) {
					if (ImGui::Button("Move")) {
						for (const auto &file: m_selected_files) {
							if (!file.second) continue;
							modal_file = *file.second;
							file_extension = std::string(modal_file.extension().string());
							file_name = std::string(modal_file.filename().replace_extension().string());

							auto from = modal_file;
							auto mod_path = std::filesystem::path(m_selected_path + '\\' + mod_name);
							move_file(from, mod_path);
						}
						close_choose();
					}
				}

				if (ImGui::Button("Close")) {
					close_choose();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}
}