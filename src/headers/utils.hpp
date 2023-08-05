#pragma once

#include <map>
#include <imgui.h>
#include <filesystem>
#include "../database/headers/ini_manager.hpp"

namespace gimi::utils {
	void draw_dirs(
			const CSimpleIniA &from_ini, const CSimpleIniA::TNamesDepend &data,
			const std::string &base, const std::string &category,
			std::string &selected, std::string &selected_path);

	void load_files(const std::filesystem::path &from, std::map<std::string, std::string> &to);

	template<typename K, typename V>
	V GetWithDef(const std::map<K, V> &m, const K &key, const V &defval) {
		typename std::map<K, V>::const_iterator it = m.find(key);
		if (it == m.end()) {
			return defval;
		} else {
			return it->second;
		}
	}

	template<typename K, typename V>
	V GetWithDefp(const std::map<K, V> &m, const K &key, V defval) {
		typename std::map<K, V>::const_iterator it = m.find(key);
		if (it == m.end()) {
			return defval;
		} else {
			return it->second;
		}
	}
}