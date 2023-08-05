#include "headers/utils.hpp"

namespace gimi::utils {
	void draw_dirs(
			const CSimpleIniA &from_ini, const CSimpleIniA::TNamesDepend &data,
			const std::string &base, const std::string &category,
			std::string &selected, std::string &selected_path) {
		for (const auto &dir: data) {
			std::filesystem::path _path = database::ini_manager::main_folder() + base + dir.pItem;
			auto _name = category + dir.pItem;

			CSimpleIniA::TNamesDepend inner;
			from_ini.GetAllValues((_name).c_str(), "category", inner);

			auto name = std::string(_path.filename().string());
			auto flags = ImGuiTreeNodeFlags_SpanAvailWidth
						 | ImGuiTreeNodeFlags_OpenOnArrow
						 | ImGuiTreeNodeFlags_OpenOnDoubleClick
						 | (selected == name
							? ImGuiTreeNodeFlags_Selected
							: 0);

			if (inner.empty()) {
				flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				ImGui::TreeNodeEx(name.c_str(), flags);
			} else if (ImGui::TreeNodeEx(name.c_str(), flags)) {
				draw_dirs(from_ini, inner,
						  base + dir.pItem, _name,
						  selected, selected_path);
				ImGui::TreePop();
				continue;
			}

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
				selected = name;
				selected_path = _path.string();
			}
		}
	}

	void load_files(const std::filesystem::path &from, std::map<std::string, std::string> &to) {
		to.clear();
		for (const auto &entry: std::filesystem::recursive_directory_iterator(from)) {
			if (!entry.is_regular_file()) continue;

			try {
				const auto &path = entry.path();
				to[std::string(path.filename().string())] = std::string(path.string());
			} catch (const std::filesystem::filesystem_error &) {
			} catch (const std::exception &) {
			}
		}
	}
}