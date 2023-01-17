#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "config.hpp"
#include "utils.hpp"


inline std::string itos(int i) {std::stringstream s; s << i; return s.str(); }
inline std::string uitos(uint i) {std::stringstream s; s << i; return s.str(); }

inline std::string getFilename(const std::string& file_path) {
    std::vector<std::string> split_path;
    boost::split(split_path, file_path, boost::is_any_of("/"));
    return split_path[split_path.size() - 1];
}

inline std::string generateName(const std::string& file_path, const std::string& type) {
    std::string filename = getFilename(file_path);
    return str(boost::format("%1%-%2%") % type % filename);
}

inline static nlohmann::json readJson(const std::string& path) {
    nlohmann::json data;
    std::ifstream ifs(path, std::ifstream::in);
    if (!ifs)
        throw std::system_error(ENOENT, std::system_category(), path);
    ifs >> data;
    return data;
}

struct coords {
    int x;
    int y;
};

