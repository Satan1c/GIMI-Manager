#pragma once

#include <string>
#include <filesystem>

namespace database::ini_manager {
	void init();

	void save();

	std::string gimi_folder();

	std::string main_folder();

	std::filesystem::path zips_folder();

	std::filesystem::path mods_folder();

	std::filesystem::path downloads_folder();
}