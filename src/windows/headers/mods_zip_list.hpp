#pragma once

#include "../database/headers/ini_manager.hpp"
#include <map>
#include <string>

namespace windows::zips {
	extern bool modal;
	extern std::map<std::string, std::string *> m_selected_files;
	extern CSimpleIniA::TNamesDepend zips_paths;
	extern std::string zips_paths_base;

	void draw();

	void reload_list();
}