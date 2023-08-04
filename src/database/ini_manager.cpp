#include "headers/ini_manager.hpp"

#include <SimpleIni.h>
#include <filesystem>
#include <fstream>

namespace database::ini_manager {
	static CSimpleIniA ini;
	//const std::string path = std::filesystem::current_path().string() + R"(\config.ini)";
	const std::string path = R"(C:\Users\Satan1c\Desktop\test\gimi_manager.ini)";
	const char *path_cstr = path.c_str();

	void init() {
		if (!std::filesystem::exists(path)) {
			std::ofstream ostrm(path);
		}
		ini.SetUnicode();
		ini.LoadFile(path_cstr);

		ini.SetValue("Paths", "gimi", gimi_folder().c_str());
		ini.SetValue("Paths", "main", main_folder().c_str());
		save();
	}

	std::string gimi_folder() {
		std::string value = ini.GetValue(
				"Paths",
				"gimi",
				std::filesystem::current_path().string().c_str());

		return value;
	}

	std::string main_folder() {
		std::string value = ini.GetValue(
				"Paths",
				"main",
				(std::filesystem::current_path().string() + R"(\gimi_manager)").c_str());

		return value;
	}

	std::filesystem::path zips_folder() {
		return main_folder() + R"(\Zips)";
	}

	std::filesystem::path mods_folder() {
		return main_folder() + R"(\Mods)";
	}

	std::filesystem::path downloads_folder() {
		return main_folder() + R"(\Download)";
	}

	void save() {
		ini.SaveFile(path_cstr);
	}
}