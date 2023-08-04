#pragma once

#include <map>
#include <filesystem>

namespace gimi::utils {
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