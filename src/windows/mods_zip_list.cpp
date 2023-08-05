#include "headers/mods_zip_list.hpp"

#include <imgui.h>
#include <filesystem>
#include "../headers/utils.hpp"
#include "headers/modals.hpp"

namespace windows::zips {
	bool modal;
	std::map<std::string, std::string *> m_selected_files = {};
	CSimpleIniA::TNamesDepend zips_paths;
	std::string zips_paths_base;

	static std::map<std::string, std::string> zips_map = {};

	void reload_list() {
		m_selected_files = {};
		gimi::utils::load_files(database::ini_manager::downloads_folder(), zips_map);
		zips_paths_base = database::ini_manager::load_zips_folder(zips_paths);
	}

	void draw_zips() {
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
	}

	void draw() {
		ImGui::Begin("Zips Window");

		if (modal && ImGui::BeginPopupModal("Choose path")) {
			windows::modals::zips::choose_move_modal(modal, zips_paths, zips_paths_base, m_selected_files);
			ImGui::EndPopup();
		}

		if (ImGui::SmallButton("Reload")) {
			reload_list();
		}

		if (ImGui::SmallButton("Move")) {
			ImGui::OpenPopup("Choose path");
			modal = true;
		}

		if (ImGui::TreeNode("Zips list")) {
			draw_zips();
			ImGui::TreePop();
		}

		ImGui::End();
	}
}