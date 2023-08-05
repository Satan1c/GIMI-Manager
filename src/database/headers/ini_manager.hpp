#pragma once

#include <string>
#include <filesystem>
#include <SimpleIni.h>

namespace database::ini_manager {
	extern CSimpleIniA ini;
	extern CSimpleIniA ini_cats;
	extern CSimpleIniA ini_mods;

	int_least8_t init();

	void save();

	std::string gimi_folder();

	std::string main_folder();

	std::string load_zips_folder(CSimpleIniA::TNamesDepend &to);

	std::string load_mods_folder(CSimpleIniA::TNamesDepend &to);

	std::filesystem::path downloads_folder();
}