#include "headers/modals.hpp"

namespace windows::modals::zips {
	static std::string m_selected;
	static std::string m_selected_path;

	static std::filesystem::path modal_file;
	static std::string file_extension;
	static std::string mod_name;
	static std::string file_name;

	static std::filesystem::copy_options options =
			std::filesystem::copy_options::recursive
			| std::filesystem::copy_options::update_existing;

	void close_choose(bool& modal) {
		modal = false;

		m_selected.clear();
		m_selected_path.clear();
		mod_name.clear();
		file_name.clear();
		file_extension.clear();

		ImGui::CloseCurrentPopup();
	}

	void move_file(const std::map<std::string, std::string *>& m_selected_files) {
		for (const auto &file: m_selected_files) {
			if (!file.second) continue;
			modal_file = *file.second;
			file_extension = std::string(modal_file.extension().string());
			file_name = std::string(modal_file.filename().replace_extension().string());

			auto from = modal_file;
			auto mod_path = std::filesystem::path(m_selected_path.append("\\").append(mod_name));
			auto to = mod_path.string().append("\\").append(file_name).append(file_extension);

			if (!exists(mod_path))
				std::filesystem::create_directories(mod_path);
			std::filesystem::copy(from, to, options);
			//std::filesystem::remove(from);
		}

	}

	void choose_move_modal(bool& modal,
						   const CSimpleIniA::TNamesDepend& zips_paths, const std::string& zips_paths_base,
						   const std::map<std::string, std::string *>& m_selected_files) {
		if (ImGui::TreeNode("Category list")) {
			gimi::utils::draw_dirs(
					database::ini_manager::ini_cats, zips_paths,
					zips_paths_base, "Categories",
					m_selected, m_selected_path);
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
				move_file(m_selected_files);
				close_choose(modal);
			}
		}

		if (ImGui::Button("Close")) {
			close_choose(modal);
		}
	}
}