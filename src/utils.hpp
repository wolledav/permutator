//
// Created by honza on 21.11.21.
//

#pragma once

#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <boost/format.hpp>
#include "config.hpp"
#include "utils.hpp"

using std::string;
using nlohmann::json;

inline std::string itos(int i) {std::stringstream s; s << i; return s.str(); }
inline std::string uitos(uint i) {std::stringstream s; s << i; return s.str(); }

inline std::string get_filename(const string& file_path) {
    std::vector<string> split_path;
    boost::split(split_path, file_path, boost::is_any_of("/"));
    return split_path[split_path.size() - 1];
}

inline std::string generate_name(const string& file_path, const string& type) {
    std::string filename = get_filename(file_path);
    return str(boost::format("%1%-%2%") % type % filename);
}

inline static json read_json(const string& path) {
    json data;
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

