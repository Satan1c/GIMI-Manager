#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>
#include "../headers/utils.hpp"

namespace windows::modals {
}

namespace windows::modals::zips {
	void choose_move_modal(bool& modal,
						   const CSimpleIniA::TNamesDepend& zips_paths, const std::string& zips_paths_base,
						   const std::map<std::string, std::string *>& m_selected_files);
}