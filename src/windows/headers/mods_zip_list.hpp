#pragma once

#include <map>
#include <filesystem>

namespace windows::zips {
	extern bool state;
	extern std::map<std::string, std::string> zips_map;

	void draw_list();
}