#include "headers/ini_manager.hpp"

#include <filesystem>
#include <fstream>
#include <vector>
#include <list>

namespace database::ini_manager {
	CSimpleIniA ini;
	CSimpleIniA ini_cats;
	CSimpleIniA ini_mods;
	//const std::string base = std::filesystem::current_path().string();
	const std::string _base = R"(C:\Users\Satan1c\Desktop\test\)";
	const std::string path = _base + R"(gimi_manager.ini)";
	const std::string path_cats = _base + R"(gimi_manager_categories.ini)";
	const std::string path_mods = _base + R"(gimi_manager_mods.ini)";

	const char *path_cstr = path.c_str();
	const char *path_cats_cstr = path_cats.c_str();
	const char *path_mods_cstr = path_mods.c_str();

	void remove_empty_dirs(
			const CSimpleIniA &from_ini, const CSimpleIniA::TNamesDepend &data,
			const std::string &base, const std::string &category) {
		for (const auto &dir: data) {
			auto name = category + dir.pItem;

			CSimpleIniA::TNamesDepend inner;
			from_ini.GetAllValues((name).c_str(), "category", inner);

			if (inner.empty()) {
				auto _path = main_folder() + base + dir.pItem;
				auto count = std::count_if(
						std::filesystem::directory_iterator(_path),
						std::filesystem::directory_iterator{},
						(bool (*)(const std::filesystem::path &)) std::filesystem::is_directory);
				if (count > 0) continue;

				std::filesystem::remove_all(_path);
			} else
				remove_empty_dirs(from_ini, inner, base + dir.pItem, name);
		}
	}

	void map_dirs(
			const CSimpleIniA &from_ini, const CSimpleIniA::TNamesDepend &data,
			const std::string &base, const std::string &category) {
		for (const auto &dir: data) {
			auto name = category + dir.pItem;

			CSimpleIniA::TNamesDepend inner;
			from_ini.GetAllValues((name).c_str(), "category", inner);

			if (inner.empty()) {
				auto _path = main_folder() + base + dir.pItem;
				std::filesystem::create_directories(_path);
			} else
				map_dirs(from_ini, inner, base + dir.pItem, name);
		}
	}

	int_least8_t init() {
		if (!std::filesystem::exists(path_cats)) {
			return 1;
		}

		if (!std::filesystem::exists(path)) {
			std::ofstream ostrm(path);
		}
		if (!std::filesystem::exists(path_mods)) {
			std::ofstream ostrm(path_mods);
		}

		ini.SetUnicode();
		ini_cats.SetUnicode();
		ini_mods.SetUnicode();

		ini_cats.SetMultiKey();
		ini_mods.SetMultiKey();

		ini.LoadFile(path_cstr);

		ini.SetValue("Paths", "gimi", gimi_folder().c_str());
		ini.SetValue("Paths", "main", main_folder().c_str());

		CSimpleIniA::TNamesDepend r_values;
		auto base = load_zips_folder(r_values);
		map_dirs(ini_cats, r_values, base, "Categories");

		base = load_mods_folder(r_values);
		map_dirs(ini_mods, r_values, base, "Mods");

		ini_cats.SetValue("Categories", "base", base.c_str());
		ini_mods.SetValue("Mods", "base", base.c_str());

		save();

		return 0;
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

	std::string load_zips_folder(CSimpleIniA::TNamesDepend &to) {
		ini_cats.Reset();
		ini_cats.LoadFile(path_cats_cstr);

		ini_cats.GetAllValues("Categories", "category", to);
		to.sort(CSimpleIniA::Entry::LoadOrder());

		return ini_cats.GetValue("Categories", "base", R"(\Zips)");
	}

	std::string load_mods_folder(CSimpleIniA::TNamesDepend &to) {
		ini_mods.Reset();
		ini_mods.LoadFile(path_mods_cstr);

		ini_mods.GetAllValues("Mods", "category", to);
		to.sort(CSimpleIniA::Entry::LoadOrder());

		return ini_mods.GetValue("Categories", "base", R"(\Mods)");
	}

	std::filesystem::path downloads_folder() {
		return main_folder() + R"(\Download)";
	}

	void save() {
		ini.SaveFile(path_cstr);
	}
}